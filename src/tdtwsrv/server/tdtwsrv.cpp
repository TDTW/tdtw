#include "tdtwsrv.h"

#include <base\system.h>

#include <engine\engine.h>
#include <engine\shared\packer.h>
#include <engine\shared\config.h>

#include <engine\kernel.h>

#include <game\generated\protocol_tdtw.h>

#include <iostream>


CTdtwSrv::CTdtwSrv()
{
	m_pGame = CreateGame();
	m_pConfig = NULL;
	m_pConsole = NULL;
	m_pEngine = NULL;
	m_pStorage = NULL;



}

CTdtwSrv::~CTdtwSrv()
{
	// save logger file
	m_pConsole->SaveLogger();

	delete m_pAutoUpdate;
}

int CTdtwSrv::SendMsg(CMsgPacker *pMsg, int Flags, int ClientID)
{
	return SendMsgEx(pMsg, Flags, ClientID, false);
}

int CTdtwSrv::SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System)
{
	CNetChunk Packet;
	if (!pMsg)
		return -1;

	mem_zero(&Packet, sizeof(CNetChunk));

	Packet.m_ClientID = ClientID;
	Packet.m_pData = pMsg->Data();
	Packet.m_DataSize = pMsg->Size();

	// HACK: modify the message id in the packet and store the system flag
	*((unsigned char*)Packet.m_pData) <<= 1;
	if (System)
		*((unsigned char*)Packet.m_pData) |= 1;

	if (Flags&MSGFLAG_VITAL)
		Packet.m_Flags |= NETSENDFLAG_VITAL;
	if (Flags&MSGFLAG_FLUSH)
		Packet.m_Flags |= NETSENDFLAG_FLUSH;

	if (!(Flags&MSGFLAG_NOSEND))
	{
		if (ClientID == -1)
		{
			// broadcast
			for (int i = 0; i < Game()->ClientsNum(); i++)
			{
				if (Game()->ClientState(i) == CClientTdtw::STATE_CONNECTED)
				{
					Packet.m_ClientID = i;
					m_NetServer.Send(&Packet);
				}
			}
		}
		else
			m_NetServer.Send(&Packet);
	}
	return 0;
}

int CTdtwSrv::NewClientCallback(int ClientID, void *pUser)
{
	CTdtwSrv *pThis = (CTdtwSrv *)pUser;
	pThis->Game()->AddClient();


	return 0;
}

int CTdtwSrv::DelClientCallback(int ClientID, const char *pReason, void *pUser)
{
	CTdtwSrv *pThis = (CTdtwSrv *)pUser;

	char aAddrStr[NETADDR_MAXSTRSIZE];
	net_addr_str(pThis->m_NetServer.ClientAddr(ClientID), aAddrStr, sizeof(aAddrStr), true);

	pThis->Console()->PrintArg(IConsole::OUTPUT_LEVEL_ADDINFO, "server",
		"client dropped. cid=%d addr=%s reason='%s'", ClientID, aAddrStr, pReason);
	
	pThis->Game()->DeleteClient(ClientID);
	return 0;
}

void CTdtwSrv::Run()
{
	// start server
	NETADDR BindAddr;
	if (g_Config.m_Bindaddr[0] && net_host_lookup(g_Config.m_Bindaddr, &BindAddr, NETTYPE_ALL) == 0)
	{
		// sweet!
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = TDTW_PORT;
	}
	else
	{
		mem_zero(&BindAddr, sizeof(BindAddr));
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = TDTW_PORT;
	}

	if (!m_NetServer.Open(BindAddr, 0/*&m_ServerBan*/, g_Config.m_SvMaxClients, g_Config.m_SvMaxClientsPerIP, 0))
	{
		m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "server", "couldn't open socket. port %d might already be in use", g_Config.m_SvPort);
		return;
	}

	m_NetServer.SetCallbacks(NewClientCallback, DelClientCallback, this);

	AutoUpdate()->CheckHash();

	while (1)
	{
		CNetChunk p;
		m_NetServer.Update();
		while (m_NetServer.Recv(&p))
		{
			if (p.m_ClientID >= 0 && p.m_ClientID < Game()->ClientsNum())
				Protocol(&p);
		}


		net_socket_read_wait(m_NetServer.Socket(), 5);
	}

	// disconnect all clients on shutdown
	for (int i = 0; i < Game()->ClientsNum(); ++i)
		m_NetServer.Drop(i, "Server shutdown");
}

void CTdtwSrv::Protocol(CNetChunk *pPacket)
{
	int ClientID = pPacket->m_ClientID;
	CUnpacker Unpacker;
	Unpacker.Reset(pPacket->m_pData, pPacket->m_DataSize);

	// unpack msgid and system flag
	int Msg = Unpacker.GetInt();
	int Sys = Msg & 1;
	Msg >>= 1;

	if (Unpacker.Error())
		return;
	if (Sys)
	{
		// system message

		if (Msg == NETMSG_PING)
		{
			CMsgPacker Msg(NETMSG_PING_REPLY);
			SendMsgEx(&Msg, MSGFLAG_VITAL, ClientID, true);
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "server", "[%d] NetPing", ClientID);
		}
		else if (Msg == NETMSG_TDTW_VERSION)
		{
			char *Version = (char *)Unpacker.GetString(CUnpacker::SANITIZE_CC);
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "[%d] Client version: %s", ClientID, Version);
			if (AutoUpdate()->CheckVersion(Version))
			{
				CMsgPacker Msg(NETMSG_TDTW_WANT_UPDATE);
				SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, ClientID, false);
			}
		}
		else
		{
			
			char aHex[] = "0123456789ABCDEF";
			char aBuf[512];

			for (int b = 0; b < pPacket->m_DataSize && b < 32; b++)
			{
				aBuf[b * 3] = aHex[((const unsigned char *)pPacket->m_pData)[b] >> 4];
				aBuf[b * 3 + 1] = aHex[((const unsigned char *)pPacket->m_pData)[b] & 0xf];
				aBuf[b * 3 + 2] = ' ';
				aBuf[b * 3 + 3] = 0;
			}

			char aBufMsg[256];
			str_format(aBufMsg, sizeof(aBufMsg), "strange message ClientID=%d msg=%d data_size=%d", ClientID, Msg, pPacket->m_DataSize);
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBufMsg);
			m_pConsole->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBuf);
			
		}
	}
	else
	{
		if (Game()->ClientState(ClientID) == CClientTdtw::STATE_EMPTY)
			return;

		void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, &Unpacker);
		if (!pRawMsg)
		{
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", 
				"dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());
			
			return;
		}
		if (Msg == NETMSGTYPE_TDTW_AUTOUPDATE)
		{
			CNetMsg_AutoUpdate *pMsg = (CNetMsg_AutoUpdate *)pRawMsg;

			
		}
		/*
		if (Msg == NETMSGTYPE_TDTW_TESTCHAT)
		{
			CNetMsg_TestChat *pMsg = (CNetMsg_TestChat *)pRawMsg;

			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "chat",
				"[%s]: %s", pMsg->m_Name, pMsg->m_pMessage);
		}*/

	}
}

void CTdtwSrv::RequestInterfaces()
{
	m_pEngine = Kernel()->RequestInterface<IEngine>();
	m_pStorage = Kernel()->RequestInterface<IStorage>();
	m_pConfig = Kernel()->RequestInterface<IConfig>();
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	m_pAutoUpdate = Kernel()->RequestInterface<IAutoUpdate>();

	m_pConsole->StoreCommands(false);
	// process pending commands


	// execute logger file	
	char ConsoleLog[125];
	char time[64];
	str_timestamp_day(time, sizeof(time));
	sprintf(ConsoleLog, "\\Logs\\TDTW-Server %s.log", time);
	m_pConsole->ExecuteLogger(ConsoleLog, IOFLAG_LOGGER);

	m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "TDTW srv", "started");
}

static ITDTWSrv *CreateTDTWServer() 
{
	CTdtwSrv *pTDTWServer = static_cast<CTdtwSrv *>(mem_alloc(sizeof(CTdtwSrv), 1));
	mem_zero(pTDTWServer, sizeof(CTdtwSrv));
	return new(pTDTWServer)CTdtwSrv;
}

int main(int argc, const char **argv)
{
//#if defined(CONF_FAMILY_WINDOWS) // TODO: ��������� �� ������ �������
	setlocale(LC_ALL, "Russian");
//#endif
	//SetConsoleOutputCP(1251);
	ITDTWSrv *pServer = CreateTDTWServer();
	IKernel *pKernel = IKernel::Create();
	IEngine *pEngine = CreateEngine("TDTW Server");
	IConsole *pConsole = CreateConsole(CFGFLAG_SERVERTDTW);
	IStorage *pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_SERVER, argc, argv);
	IConfig *pConfig = CreateConfig();
	IAutoUpdate *pAutoUpdate = CreateAutoUpdate();
	{
		bool RegisterFail = false;

		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pServer);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pEngine);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pConsole);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pStorage);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pConfig);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pAutoUpdate);

		if (RegisterFail)
			::exit(0);
	}
	pEngine->Init();
	pConfig->Init();

	pServer->RequestInterfaces();

	pAutoUpdate->RequestInterfaces();

	pEngine->InitLogfile();

	
	pServer->Run();

	delete pKernel;
	delete pEngine;
	delete pStorage;
	delete pConfig;
	delete pConsole;
	delete pServer;
	return 0;
}