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
			const char *pError = 0;

			if (Unpacker.Error())
				return;

			for (int i = 0; pMap[i]; i++) // protect the player from nasty map names
			{
				if (pMap[i] == '/' || pMap[i] == '\\')
					pError = "strange character in map name";
			}

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
				//str_copy(m_aMapdownloadName, pMap, sizeof(m_aMapdownloadName));
				if (m_FileHandle)
					io_close(m_FileHandle);
				m_FileHandle = io_open(pMap, IOFLAG_WRITE);
				m_FileCrc = MapCrc;
				m_FileTotalSize = MapSize;
				m_FileDownloadAmount = 0;

				CMsgPacker Msg(NETMSG_TDTW_UPDATE_REQUEST);
				Msg.AddInt(m_FileChunk);
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
			int Last = Unpacker.GetInt();
			int MapCRC = Unpacker.GetInt();
			int Chunk = Unpacker.GetInt();
			int Size = Unpacker.GetInt();
			const unsigned char *pData = Unpacker.GetRaw(Size);

			// check for errors
			if (Unpacker.Error() || Size <= 0 || MapCRC != m_FileCrc
				|| Chunk != m_FileChunk 
				|| !m_FileHandle)
				return;

			io_write(m_FileHandle, pData, Size);

			m_FileDownloadAmount += Size;

			if (Last)
			{
				const char *pError;
				m_pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "client/network", "download complete, loading map");

				if (m_FileHandle)
					io_close(m_FileHandle);
				m_FileHandle = 0;
				m_FileDownloadAmount = 0;
				m_FileTotalSize = -1;

				// load map
				//pError = LoadMap(m_aMapdownloadName, m_aMapdownloadFilename, m_MapdownloadCrc);
			}
			else
			{
				// request new chunk
				m_FileChunk++;

				CMsgPacker Msg(NETMSG_TDTW_UPDATE_REQUEST);
				Msg.AddInt(m_FileChunk);
				Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);

				if (g_Config.m_Debug)
				{
					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "requested chunk %d", m_FileChunk);
					m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "client/network", aBuf);
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