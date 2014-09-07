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

		if (Msg == NETMSG_TDTW_WANT_UPDATE)
		{
			
		}

	}
}

ITDTWServer *CreateTDTWServer() { return new CTDTWServer; }