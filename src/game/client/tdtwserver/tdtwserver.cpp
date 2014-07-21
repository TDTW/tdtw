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
		if (Msg == NETMSG_VERSION)
		{
			m_Version = (char *)Unpacker.GetString(CUnpacker::SANITIZE_CC);
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "tdtwserver", "Latest version: %s", m_Version);

			CMsgPacker Msg(NETMSG_VERSION);
			Msg.AddString(GAME_VERSION, 64);
			Msg.AddString(g_Config.m_PlayerName, 16);
			Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
		}
	}
}

ITDTWServer *CreateTDTWServer() { return new CTDTWServer; }