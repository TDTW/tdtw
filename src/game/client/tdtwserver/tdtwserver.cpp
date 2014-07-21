#include "tdtwserver.h"


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
		void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, &Unpacker);
		if (!pRawMsg)
		{
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server",
				"dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());

			return;
		}
		if (Msg == NETMSGTYPE_SYS_TDTW_VERSION)
		{
			CNetMsg_Version *pMsg = (CNetMsg_Version *)pRawMsg;
			m_Version = (char *)pMsg->m_Version;
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "Version of TDTW: %s", m_Version);
			CNetMsg_Version Msg123;
			Msg123.m_Version = GAME_VERSION;			
			Client()->SendPackMsg(&Msg123, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
		}
	}
}

ITDTWServer *CreateTDTWServer() { return new CTDTWServer; }