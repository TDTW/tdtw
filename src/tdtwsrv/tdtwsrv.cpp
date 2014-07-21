#include "tdtwsrv.h"
#include "client.h"

#include <base\system.h>

#include <engine\engine.h>
#include <engine\shared\packer.h>
#include <engine\shared\config.h>

#include <engine\kernel.h>

#include <game\generated\protocol_tdtw.h>

#include <iostream>


TdtwSrv::TdtwSrv(int argc, const char **argv)
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

	dbg_msg("TDTW srv", "started");
}

TdtwSrv::~TdtwSrv()
{
	delete pKernel;
	delete pEngine;
	delete pStorage;
	delete pConfig;
	delete m_pConsole;
}

int TdtwSrv::SendMsg(CMsgPacker *pMsg, int Flags, int ClientID)
{
	return SendMsgEx(pMsg, Flags, ClientID, false);
}

int TdtwSrv::SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System)
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

int TdtwSrv::NewClientCallback(int ClientID, void *pUser)
{
	TdtwSrv *pThis = (TdtwSrv *)pUser;
	CClientTdtw *NewClient = new CClientTdtw;
	pThis->m_aClients.add(NewClient);
	CNetMsg_Version Msg;
	Msg.m_Version = GAME_VERSION;
	pThis->SendPackMsg(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, ClientID, true);
	return 0;
}

int TdtwSrv::DelClientCallback(int ClientID, const char *pReason, void *pUser)
{
	TdtwSrv *pThis = (TdtwSrv *)pUser;

	char aAddrStr[NETADDR_MAXSTRSIZE];
	net_addr_str(pThis->m_NetServer.ClientAddr(ClientID), aAddrStr, sizeof(aAddrStr), true);

	pThis->Console()->PrintArg(IConsole::OUTPUT_LEVEL_ADDINFO, "server",
		"client dropped. cid=%d addr=%s reason='%s'", ClientID, aAddrStr, pReason);
	
	pThis->m_aClients.remove_index(ClientID);
	return 0;
}

int TdtwSrv::Run()
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
		dbg_msg("server", "couldn't open socket. port %d might already be in use", g_Config.m_SvPort);
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

void TdtwSrv::Protocol(CNetChunk *pPacket)
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
/*
		if (Msg == NETMSG_INFO)
		{
			if (m_aClients[ClientID].m_State == CClient::STATE_AUTH)
			{
				const char *pVersion = Unpacker.GetString(CUnpacker::SANITIZE_CC);
				if (str_comp(pVersion, GameServer()->NetVersion()) != 0)
				{
					// wrong version
					char aReason[256];
					str_format(aReason, sizeof(aReason), "Wrong version. Server is running '%s' and client '%s'", GameServer()->NetVersion(), pVersion);
					m_NetServer.Drop(ClientID, aReason);
					return;
				}

				const char *pPassword = Unpacker.GetString(CUnpacker::SANITIZE_CC);
				if (g_Config.m_Password[0] != 0 && str_comp(g_Config.m_Password, pPassword) != 0)
				{
					// wrong password
					m_NetServer.Drop(ClientID, "Wrong password");
					return;
				}

				m_aClients[ClientID].m_State = CClient::STATE_CONNECTING;
				SendMap(ClientID);
			}
		}
		else if (Msg == NETMSG_REQUEST_MAP_DATA)
		{
			if (m_aClients[ClientID].m_State < CClient::STATE_CONNECTING)
				return;

			int Chunk = Unpacker.GetInt();
			int ChunkSize = 1024 - 128;
			int Offset = Chunk * ChunkSize;
			int Last = 0;

			// drop faulty map data requests
			if (Chunk < 0 || Offset > m_CurrentMapSize)
				return;

			if (Offset + ChunkSize >= m_CurrentMapSize)
			{
				ChunkSize = m_CurrentMapSize - Offset;
				if (ChunkSize < 0)
					ChunkSize = 0;
				Last = 1;
			}

			CMsgPacker Msg(NETMSG_MAP_DATA);
			Msg.AddInt(Last);
			Msg.AddInt(m_CurrentMapCrc);
			Msg.AddInt(Chunk);
			Msg.AddInt(ChunkSize);
			Msg.AddRaw(&m_pCurrentMapData[Offset], ChunkSize);
			SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, ClientID, true);

			if (g_Config.m_Debug)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "sending chunk %d with size %d", Chunk, ChunkSize);
				Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBuf);
			}
		}
		else if (Msg == NETMSG_READY)
		{
			if (m_aClients[ClientID].m_State == CClient::STATE_CONNECTING)
			{
				char aAddrStr[NETADDR_MAXSTRSIZE];
				net_addr_str(m_NetServer.ClientAddr(ClientID), aAddrStr, sizeof(aAddrStr), true);

				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "player is ready. ClientID=%x addr=%s", ClientID, aAddrStr);
				Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "server", aBuf);
				m_aClients[ClientID].m_State = CClient::STATE_READY;
				GameServer()->OnClientConnected(ClientID);
				SendConnectionReady(ClientID);
			}
		}
		else if (Msg == NETMSG_ENTERGAME)
		{
			if (m_aClients[ClientID].m_State == CClient::STATE_READY && GameServer()->IsClientReady(ClientID))
			{
				char aAddrStr[NETADDR_MAXSTRSIZE];
				net_addr_str(m_NetServer.ClientAddr(ClientID), aAddrStr, sizeof(aAddrStr), true);

				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "player has entered the game. ClientID=%x addr=%s", ClientID, aAddrStr);
				Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
				m_aClients[ClientID].m_State = CClient::STATE_INGAME;
				GameServer()->OnClientEnter(ClientID);
			}
		}
		else if (Msg == NETMSG_INPUT)
		{
			CClient::CInput *pInput;
			int64 TagTime;

			m_aClients[ClientID].m_LastAckedSnapshot = Unpacker.GetInt();
			int IntendedTick = Unpacker.GetInt();
			int Size = Unpacker.GetInt();

			// check for errors
			if (Unpacker.Error() || Size / 4 > MAX_INPUT_SIZE)
				return;

			if (m_aClients[ClientID].m_LastAckedSnapshot > 0)
				m_aClients[ClientID].m_SnapRate = CClient::SNAPRATE_FULL;

			if (m_aClients[ClientID].m_Snapshots.Get(m_aClients[ClientID].m_LastAckedSnapshot, &TagTime, 0, 0) >= 0)
				m_aClients[ClientID].m_Latency = (int)(((time_get() - TagTime) * 1000) / time_freq());

			// add message to report the input timing
			// skip packets that are old
			if (IntendedTick > m_aClients[ClientID].m_LastInputTick)
			{
				int TimeLeft = ((TickStartTime(IntendedTick) - time_get()) * 1000) / time_freq();

				CMsgPacker Msg(NETMSG_INPUTTIMING);
				Msg.AddInt(IntendedTick);
				Msg.AddInt(TimeLeft);
				SendMsgEx(&Msg, 0, ClientID, true);
			}

			m_aClients[ClientID].m_LastInputTick = IntendedTick;

			pInput = &m_aClients[ClientID].m_aInputs[m_aClients[ClientID].m_CurrentInput];

			if (IntendedTick <= Tick())
				IntendedTick = Tick() + 1;

			pInput->m_GameTick = IntendedTick;

			for (int i = 0; i < Size / 4; i++)
				pInput->m_aData[i] = Unpacker.GetInt();

			mem_copy(m_aClients[ClientID].m_LatestInput.m_aData, pInput->m_aData, MAX_INPUT_SIZE*sizeof(int));

			m_aClients[ClientID].m_CurrentInput++;
			m_aClients[ClientID].m_CurrentInput %= 200;

			// call the mod with the fresh input data
			if (m_aClients[ClientID].m_State == CClient::STATE_INGAME)
				GameServer()->OnClientDirectInput(ClientID, m_aClients[ClientID].m_LatestInput.m_aData);
		}
		else if (Msg == NETMSG_RCON_CMD)
		{
			const char *pCmd = Unpacker.GetString();

			if (Unpacker.Error() == 0 && m_aClients[ClientID].m_Authed)
			{
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "ClientID=%d rcon='%s'", ClientID, pCmd);
				Console()->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "server", aBuf);
				m_RconClientID = ClientID;
				m_RconAuthLevel = m_aClients[ClientID].m_Authed;
				Console()->SetAccessLevel(m_aClients[ClientID].m_Authed == AUTHED_ADMIN ? IConsole::ACCESS_LEVEL_ADMIN : IConsole::ACCESS_LEVEL_MOD);
				Console()->ExecuteLineFlag(pCmd, CFGFLAG_SERVER);
				Console()->SetAccessLevel(IConsole::ACCESS_LEVEL_ADMIN);
				m_RconClientID = IServer::RCON_CID_SERV;
				m_RconAuthLevel = AUTHED_ADMIN;
			}
		}
		else if (Msg == NETMSG_RCON_AUTH)
		{
			const char *pPw;
			Unpacker.GetString(); // login name, not used
			pPw = Unpacker.GetString(CUnpacker::SANITIZE_CC);

			if (Unpacker.Error() == 0)
			{
				if (g_Config.m_SvRconPassword[0] == 0 && g_Config.m_SvRconModPassword[0] == 0)
				{
					SendRconLine(ClientID, "No rcon password set on server. Set sv_rcon_password and/or sv_rcon_mod_password to enable the remote console.");
				}
				else if (g_Config.m_SvRconPassword[0] && str_comp(pPw, g_Config.m_SvRconPassword) == 0)
				{
					CMsgPacker Msg(NETMSG_RCON_AUTH_STATUS);
					Msg.AddInt(1);	//authed
					Msg.AddInt(1);	//cmdlist
					SendMsgEx(&Msg, MSGFLAG_VITAL, ClientID, true);

					m_aClients[ClientID].m_Authed = AUTHED_ADMIN;
					int SendRconCmds = Unpacker.GetInt();
					if (Unpacker.Error() == 0 && SendRconCmds)
						m_aClients[ClientID].m_pRconCmdToSend = Console()->FirstCommandInfo(IConsole::ACCESS_LEVEL_ADMIN, CFGFLAG_SERVER);
					SendRconLine(ClientID, "Admin authentication successful. Full remote console access granted.");
					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "ClientID=%d authed (admin)", ClientID);
					Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
				}
				else if (g_Config.m_SvRconModPassword[0] && str_comp(pPw, g_Config.m_SvRconModPassword) == 0)
				{
					CMsgPacker Msg(NETMSG_RCON_AUTH_STATUS);
					Msg.AddInt(1);	//authed
					Msg.AddInt(1);	//cmdlist
					SendMsgEx(&Msg, MSGFLAG_VITAL, ClientID, true);

					m_aClients[ClientID].m_Authed = AUTHED_MOD;
					int SendRconCmds = Unpacker.GetInt();
					if (Unpacker.Error() == 0 && SendRconCmds)
						m_aClients[ClientID].m_pRconCmdToSend = Console()->FirstCommandInfo(IConsole::ACCESS_LEVEL_MOD, CFGFLAG_SERVER);
					SendRconLine(ClientID, "Moderator authentication successful. Limited remote console access granted.");
					char aBuf[256];
					str_format(aBuf, sizeof(aBuf), "ClientID=%d authed (moderator)", ClientID);
					Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "server", aBuf);
				}
				else if (g_Config.m_SvRconMaxTries)
				{
					m_aClients[ClientID].m_AuthTries++;
					char aBuf[128];
					str_format(aBuf, sizeof(aBuf), "Wrong password %d/%d.", m_aClients[ClientID].m_AuthTries, g_Config.m_SvRconMaxTries);
					SendRconLine(ClientID, aBuf);
					if (m_aClients[ClientID].m_AuthTries >= g_Config.m_SvRconMaxTries)
					{
						if (!g_Config.m_SvRconBantime)
							m_NetServer.Drop(ClientID, "Too many remote console authentication tries");
						else
							m_ServerBan.BanAddr(m_NetServer.ClientAddr(ClientID), g_Config.m_SvRconBantime * 60, "Too many remote console authentication tries");
					}
				}
				else
				{
					SendRconLine(ClientID, "Wrong password.");
				}
			}
		}
		else*/
		if (Msg == NETMSG_PING)
		{
			CMsgPacker Msg(NETMSG_PING_REPLY);
			SendMsgEx(&Msg, MSGFLAG_VITAL, ClientID, true);
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "server", "[%d] NetPing", ClientID);
		}
		else if (Msg == NETMSGTYPE_SYS_TDTW_VERSION)
		{
			void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, &Unpacker);
			if (!pRawMsg)
			{
				m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server",
					"dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());
				return;
			}
			CNetMsg_Version *pMsg = (CNetMsg_Version *)pRawMsg;
			m_pConsole->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "Client %d tdtw version: %s", ClientID, pMsg->m_Version);
		}
		else
		{
			//if (g_Config.m_Debug)
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
				Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBufMsg);
				Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBuf);
			}
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

		if (Msg == NETMSGTYPE_SYS_TDTW_TESTCHAT)
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
	TdtwSrv *TdtwServer = new TdtwSrv(argc, argv);
	return TdtwServer->Run();
}