#ifndef TDTWSRV_H
#define TDTWSRV_H

#include <base\system.h>

#include <engine\shared\network.h>
#include <engine\shared\netban.h>

#include <engine\console.h>
#include <engine\storage.h>
#include <engine\config.h>

#include <game\version.h>
#include <game\generated\protocol_tdtw.h>

#include "autoupdate.h"
#include "client.h"

class CTdtwSrv
{
public:
	CTdtwSrv(int argc, const char **argv);
	~CTdtwSrv();

private:
	class IKernel *pKernel;
	class IEngine *pEngine;
	class IStorage *pStorage;
	class IConfig *pConfig;
	class IConsole *m_pConsole;

	int m_CurrentGameTick;
	int m_TickSpeed;
public:
	class IStorage *Storage() { return pStorage; }
	class IConfig *Config() { return pConfig; }
	class IConsole *Console() { return m_pConsole; }

	int Tick() const { return m_CurrentGameTick; }
	int TickSpeed() const { return m_TickSpeed; }

	int MaxClients() const { return m_NetServer.MaxClients(); }
	int OnlineCount() const { return m_aClients.size(); }

	int SendMsg(CMsgPacker *pMsg, int Flags, int ClientID);
	int SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System);

	template<class T>
	int SendPackMsg(T *pMsg, int Flags, int ClientID, bool System = false)
	{
		CMsgPacker Packer(pMsg->MsgID());
		if (pMsg->Pack(&Packer))
			return -1;
		return SendMsgEx(&Packer, Flags, ClientID, System);
	}
	enum
	{
		AUTHED_NO = 0,
		AUTHED_MOD,
		AUTHED_ADMIN,

		MAX_RCONCMD_SEND = 16,
	};

	// client class here (name, clan, ping, friends, score and so.on.)
	array <CClientTdtw*> m_aClients;

	// server class here (name, map, type, online, score and so.on.)

	int Run();
	void Protocol(CNetChunk *Chunk);

	static int NewClientCallback(int ClientID, void *pUser);
	static int DelClientCallback(int ClientID, const char *pReason, void *pUser);

	/* TODO: Add ban functions
	InitServerBan
	BanExt
	BanAddr
	BanRange
	ConBanExt

	*/
	/* TODO: Add admin functions
	Kick
	*/
	class CAutoUpdate *AutoUpdate() { return m_pAutoUpdate; }
private:
	CNetTdtwServer m_NetServer;
	CNetObjHandlerTdtw m_NetHandler;
	CAutoUpdate *m_pAutoUpdate;
//	CServerBan m_ServerBan;
	
};
#endif