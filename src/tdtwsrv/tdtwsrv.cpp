#include "tdtwsrv.h"
#include "client.h"

#include <base\system.h>

#include <engine\engine.h>
#include <engine\shared\packer.h>
#include <engine\shared\config.h>

#include <engine\kernel.h>

#include <game\generated\protocol_tdtw.h>

#include <iostream>


CTdtwSrv::CTdtwSrv(int argc, const char **argv)
{
	pKernel = IKernel::Create();
	pEngine = CreateEngine("TDTW Server");
	m_pConsole = CreateConsole(CFGFLAG_MASTER | CFGFLAG_ECON);
	pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_SERVER, argc, argv);
	pConfig = CreateConfig();

	m_aClients.clear();

	m_CurrentGameTick = 0;
	m_TickSpeed = SERVER_TICK_SPEED;
/*	// TODO: ADD
	m_RconClientID = IServer::RCON_CID_SERV;
	m_RconAuthLevel = AUTHED_ADMIN;*/
	{
		bool RegisterFail = false;

		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pEngine);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(m_pConsole);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pStorage);
		RegisterFail = RegisterFail || !pKernel->RegisterInterface(pConfig);

		if (RegisterFail)
			::exit(0);
	}

	pEngine->Init();
	pConfig->Init();

	//RegisterCommands(); // TODO: ADD from server.cpp
/*	m_pConsole->ExecuteFile("tdtwexec.cfg"); // TODO: Make config
	// parse the command line arguments

	if (argc > 1) // ignore_convention
		m_pConsole->ParseArguments(argc - 1, &argv[1]); // ignore_convention

	// restore empty config strings to their defaults
	pConfig->RestoreStrings();*/
	
	pEngine->InitLogfile();

	// process pending commands
	m_pConsole->StoreCommands(false);

	// execute logger file	
	char ConsoleLog[125];
	char time[64];
	str_timestamp_day(time, sizeof(time));
	sprintf(ConsoleLog, "\\Logs\\TDTW-Server %s.log", time);

	m_pConsole->ExecuteLogger(ConsoleLog, IOFLAG_LOGGER);

	m_pAutoUpdate = new CAutoUpdate(this, Storage());
	AutoUpdate()->CheckHash();
	m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "TDTW srv", "started");
}

CTdtwSrv::~CTdtwSrv()
{
	// save logger file
	m_pConsole->SaveLogger();

	delete pKernel;
	delete pEngine;
	delete pStorage;
	delete pConfig;
	delete m_pConsole;
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
			for (int i = 0; i < m_aClients.size(); i++)
			{
				if (m_aClients[i]->m_State == CClientTdtw::STATE_CONNECTED)
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
	CClientTdtw *NewClient = new CClientTdtw;
	pThis->m_aClients.add(NewClient);

	CMsgPacker Msg(NETMSG_VERSION);
	Msg.AddString(GAME_VERSION, 64);
	pThis->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, ClientID, true);
	return 0;
}

int CTdtwSrv::DelClientCallback(int ClientID, const char *pReason, void *pUser)
{
	CTdtwSrv *pThis = (CTdtwSrv *)pUser;

	char aAddrStr[NETADDR_MAXSTRSIZE];
	net_addr_str(pThis->m_NetServer.ClientAddr(ClientID), aAddrStr, sizeof(aAddrStr), true);

	pThis->Console()->PrintArg(IConsole::OUTPUT_LEVEL_ADDINFO, "server",
		"client dropped. cid=%d addr=%s reason='%s'", ClientID, aAddrStr, pReason);
	
	pThis->m_aClients.remove_index(ClientID);
	return 0;
}

int CTdtwSrv::Run()
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
		return -1;
	}

	m_NetServer.SetCallbacks(NewClientCallback, DelClientCallback, this);

	//m_Econ.Init(Console(), &m_ServerBan); // TODO: Make this

	while (1)
	{
		CNetChunk p;
		m_NetServer.Update();
		while (m_NetServer.Recv(&p))
		{
			if (p.m_ClientID >= 0 && p.m_ClientID < m_aClients.size())
				Protocol(&p);
		}

		//m_ServerBan.Update();
		//m_Econ.Update();	// TODO: Make this

		net_socket_read_wait(m_NetServer.Socket(), 5);
	}

	// disconnect all clients on shutdown
	for (int i = 0; i < m_aClients.size(); ++i)
	{
		m_NetServer.Drop(i, "Server shutdown");

		//m_Econ.Shutdown();	// TODO: Make this
	}
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
		else if (Msg == NETMSG_VERSION)
		{
			char *Version = (char *)Unpacker.GetString(CUnpacker::SANITIZE_CC);
			char *Name = (char *)Unpacker.GetString(CUnpacker::SANITIZE_CC);
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "[%d]:[%s] Client version: %s", ClientID, Name, Version);
			str_copy(m_aClients[ClientID]->m_aName, Name, sizeof(Name));
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
		if (m_aClients[ClientID]->m_State == CClientTdtw::STATE_EMPTY)
			return;

		void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, &Unpacker);
		if (!pRawMsg)
		{
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", 
				"dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());
			
			return;
		}

		if (Msg == NETMSGTYPE_TDTW_TESTCHAT)
		{
			CNetMsg_TestChat *pMsg = (CNetMsg_TestChat *)pRawMsg;

			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "chat",
				"[%s]: %s", pMsg->m_Name, pMsg->m_pMessage);
		}

	}
}

int main(int argc, const char **argv)
{
//#if defined(CONF_FAMILY_WINDOWS) // TODO: проверить на другие системы
	setlocale(LC_ALL, "Russian");
//#endif
	//SetConsoleOutputCP(1251);
	CTdtwSrv *TdtwServer = new CTdtwSrv(argc, argv);
	return TdtwServer->Run();
}