#include "tdtwserver.h"
#include <engine/shared/config.h>

CTDTWServer::CTDTWServer()
{

}
void CTDTWServer::Init()
{
	m_Version = 0x0;
	m_pClient = Kernel()->RequestInterface<IClient>();
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	m_pAutoUpdate = Kernel()->RequestInterface<IAutoUpdate>();
	m_FileHandle = 0;
	tempInt = 0;

}
void CTDTWServer::Recv(CNetChunk *pChunk)
{
	if (pChunk->m_ClientID != -1)
		Protocol(pChunk);
}

void CTDTWServer::Protocol(CNetChunk *pChunk)
{
	int ClientID = pChunk->m_ClientID;
	CUnpacker Unpacker;
	Unpacker.Reset(pChunk->m_pData, pChunk->m_DataSize);

	// unpack msgid and system flag
	int Msg = Unpacker.GetInt();
	int Sys = Msg & 1;
	Msg >>= 1;

	if (Unpacker.Error())
		return;

	if (Sys)
	{
		if (Msg == NETMSG_TDTW_UPDATE_INFO)
		{
			const char *pMap = Unpacker.GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			int MapCrc = Unpacker.GetInt();
			int MapSize = Unpacker.GetInt();
			int FileChunks = Unpacker.GetInt();
			const char *pError = 0;

			if (Unpacker.Error())
				return;

			if (MapSize < 0)
				pError = "invalid map size";
			if (pError)
				m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "client/network/tdtw/autoupdater", "Error:%s", pError);
			else
			{
				//str_format(m_aMapdownloadFilename, sizeof(m_aMapdownloadFilename), "downloadedmaps/%s_%08x.map", pMap, MapCrc);

				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "starting to download map to '%s'", pMap);
				m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "client/network", aBuf);

				m_FileChunk = 0;
				m_FileTotalChunks = FileChunks;
				//str_copy(m_aMapdownloadName, pMap, sizeof(m_aMapdownloadName));
				if (m_FileHandle)
					io_close(m_FileHandle);
				m_FileHandle = io_open(pMap, IOFLAG_WRITE);
				m_FileCrc = MapCrc;
				m_FileTotalSize = MapSize;
				m_FileDownloadAmount = 0;

				CMsgPacker Msg(NETMSG_TDTW_UPDATE_REQUEST);
				Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);

				if (g_Config.m_Debug)
				{
					str_format(aBuf, sizeof(aBuf), "requested chunk %d", m_FileChunk);
					m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client/network", aBuf);
				}
				
			}
		}
		else if (Msg == NETMSG_TDTW_UPDATE_DATA)
		{
			int Chunk = Unpacker.GetInt();
			int Size = Unpacker.GetInt();
			const unsigned char *pData = Unpacker.GetRaw(Size);

			// check for errors
			if (Unpacker.Error() || Size <= 0 || Chunk != m_FileChunk 
				|| !m_FileHandle)
				return;

			m_FileChunk++;
			m_FileDownloadAmount += Size;

			dbg_msg("UPD", "%d/%d", m_FileDownloadAmount, m_FileTotalSize);
			dbg_msg("Updater", "%d", m_FileChunk);

			io_write(m_FileHandle, pData, Size);
			

			if (m_FileDownloadAmount == m_FileTotalSize)
			{
				const char *pError;
				m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "client/network", "download complete, loading map");

				if (m_FileHandle)
					io_close(m_FileHandle);

				m_FileHandle = 0;
				m_FileDownloadAmount = 0;
				m_FileTotalSize = -1;
				m_FileChunk = 0;
				m_FileTotalChunks = 0;
				AutoUpdate()->SetNeedReplace(true);
			}
			else
			{
				if ((m_FileChunk + 1) % 10 == 0)
				{
					CMsgPacker Msg(NETMSG_TDTW_UPDATE_REQUEST);
					Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
				}
			}			
		}
		else if (Msg == NETMSG_TDTW_HASH_REQUEST)
		{
			const char *pFile = Unpacker.GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			if (str_comp(".", pFile))
			{
				for (int i = 0; i < AutoUpdate()->m_aDir.size(); i++)
				{
					if (str_comp(AutoUpdate()->m_aDir[i].Name, pFile) == 0)
					{
						for (int j = 0; j < AutoUpdate()->m_aDir[i].m_aFiles.size(); j++)
						{
							CNetMsg_AutoUpdate_Hash Msg;
							Msg.m_Name = AutoUpdate()->m_aDir[i].m_aFiles[j].Name;

							if (AutoUpdate()->m_aDir[i].m_aFiles[j].IsFolder)
								Msg.m_Hash = AutoUpdate()->m_aDir[AutoUpdate()->m_aDir[i].m_aFiles[j].FolderID].Hash;
							else
								Msg.m_Hash = AutoUpdate()->m_aDir[i].m_aFiles[j].Hash;
							dbg_msg("Client","%d", Msg.m_Hash);
							Msg.m_IsFolder = AutoUpdate()->m_aDir[i].m_aFiles[j].IsFolder;

							if (!Msg.m_IsFolder)
								Msg.m_Size = AutoUpdate()->m_aDir[i].m_aFiles[j].Size;
							else
								Msg.m_Size = 0;

							Client()->SendPackMsg(&Msg, MSGFLAG_FLUSH , false, true);
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < AutoUpdate()->m_aDir[0].m_aFiles.size(); i++)
				{
					CNetMsg_AutoUpdate_Hash Msg;
					Msg.m_Name = AutoUpdate()->m_aDir[0].m_aFiles[i].Name;
					
					if (AutoUpdate()->m_aDir[0].m_aFiles[i].IsFolder)
						Msg.m_Hash = AutoUpdate()->m_aDir[AutoUpdate()->m_aDir[0].m_aFiles[i].FolderID].Hash;
					else
						Msg.m_Hash = AutoUpdate()->m_aDir[0].m_aFiles[i].Hash;
					dbg_msg("Client","%d", Msg.m_Hash);
					Msg.m_IsFolder = AutoUpdate()->m_aDir[0].m_aFiles[i].IsFolder;
					Msg.m_Size = AutoUpdate()->m_aDir[0].m_aFiles[i].Size;

					Client()->SendPackMsg(&Msg, MSGFLAG_FLUSH , false, true);
				}
			}
		}
	}
	else
	{
		void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, &Unpacker);
		if (!pRawMsg)
		{
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "client",
				"dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());

			return;
		}
	}
}

ITDTWServer *CreateTDTWServer() { return new CTDTWServer; }