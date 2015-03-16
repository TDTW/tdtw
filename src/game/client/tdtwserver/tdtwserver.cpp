#include "tdtwserver.h"
#include <engine/shared/config.h>
#include <direct.h>

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
	UpdateFiles = new CUpdateFiles(this);
}
void CTDTWServer::Recv(CNetChunk *pChunk)
{
	if (pChunk->m_ClientID != -1)
		Protocol(pChunk);
}

void CTDTWServer::Protocol(CNetChunk *pChunk)
{
	int ClientID = pChunk->m_ClientID;
	CUnpacker Unpacker = CUnpacker();
	Unpacker.Reset(pChunk->m_pData, pChunk->m_DataSize);

	// unpack msgid and system flag
	int Msg = Unpacker.GetInt();
	int Sys = Msg & 1;
	Msg >>= 1;

	if (Unpacker.Error())
	{
		dbg_msg("123", "321");
		return;
	}

	if (Sys)
	{
		if (Msg == NETMSG_TDTW_UPDATE_INFO)
		{
			const char *FileName = Unpacker.GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
			int MapCrc = Unpacker.GetInt();
			int MapSize = Unpacker.GetInt();
			int FileChunks = Unpacker.GetInt();
			const char *pError = 0;

			if (Unpacker.Error())
				return;

			if (MapSize < 0)
				pError = "invalid map size";
			if (pError)
				m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "updater", "Error:%s", pError);
			else
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "starting to download file to '%s'", FileName);
				m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "updater", aBuf);

				m_FileChunk = 0;
				m_FileTotalChunks = FileChunks;
				if (m_FileHandle)
					io_close(m_FileHandle);
				m_FileHandle = io_open(FileName, IOFLAG_WRITE);
				m_FileCrc = MapCrc;
				m_FileTotalSize = MapSize;
				m_FileDownloadAmount = 0;

				CMsgPacker msgPacker(NETMSG_TDTW_UPDATE_REQUEST);
				Client()->SendMsgEx(&msgPacker, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);

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

			io_write(m_FileHandle, pData, Size);
			

			if (m_FileDownloadAmount == m_FileTotalSize)
			{
				const char *pError;
				m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "updater", "File successfully donwloaded");

				if (m_FileHandle)
					io_close(m_FileHandle);

				m_FileHandle = 0;
				m_FileDownloadAmount = 0;
				m_FileTotalSize = -1;
				m_FileChunk = 0;
				m_FileTotalChunks = 0;
				AutoUpdate()->SetNeedReplace(true);
				UpdateFiles->EndUpdate();
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
			if (str_comp("1", pFile) == 0)
			{
				CMsgPacker Msg(NETMSG_TDTW_HASH_REQUEST);
				Msg.AddString(".", -1);
				Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
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
		if (Msg == NETMSGTYPE_TDTW_AUTOUPDATE_HASH)
		{
			CNetMsg_AutoUpdate_Hash *Msg = (CNetMsg_AutoUpdate_Hash *)pRawMsg;

			if (Msg->m_IsFolder)
			{
				bool Find = false;
				for (int i = 0; i < AutoUpdate()->m_aDir.size(); i++)
				{
					if (str_comp(AutoUpdate()->m_aDir[i].Name, Msg->m_Name) == 0)
					{
						Find = true;
						if (AutoUpdate()->m_aDir[i].Hash != Msg->m_Hash)
						{
							CMsgPacker Ms(NETMSG_TDTW_HASH_REQUEST);
							Ms.AddString(Msg->m_Name, -1);
							Client()->SendMsgEx(&Ms, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
							break;
						}
					}
				}
				if (!Find)
				{
					CMsgPacker Ms(NETMSG_TDTW_HASH_REQUEST);
					Ms.AddString(Msg->m_Name, -1);
					Client()->SendMsgEx(&Ms, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
					mkdir(Msg->m_Name);
				}
			}
			else
			{
				bool Find = false;
				for (int i = 0; i < AutoUpdate()->m_aDir.size(); i++)
				{
					for (int j = 0; j < AutoUpdate()->m_aDir[i].m_aFiles.size(); j++)
					{
						if (str_comp(Msg->m_Name, AutoUpdate()->m_aDir[i].m_aFiles[j].Name) == 0)
						{
							Find = true;
							if (Msg->m_Hash != AutoUpdate()->m_aDir[i].m_aFiles[j].Hash)
							{
								UpdateFiles->AddFile(Msg->m_Name);
								UpdateFiles->StartUpdate();
							}
							return;
						}
					}
				}
				if (!Find)
				{
					UpdateFiles->AddFile(Msg->m_Name);
					UpdateFiles->StartUpdate();
				}
			}
		}
	}
}

ITDTWServer *CreateTDTWServer() { return new CTDTWServer; }