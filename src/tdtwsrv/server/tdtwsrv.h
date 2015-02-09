#ifndef TDTWSRV_TDTWSRV_TDTWSRV_H
#define TDTWSRV_TDTWSRV_TDTWSRV_H

#include <base/system.h>

#include <engine/shared/network.h>
#include <engine/shared/netban.h>
#include <engine/shared/protocol.h>

#include <engine/console.h>
#include <engine/storage.h>
#include <engine/config.h>
#include <engine/autoupdate.h>

#include <engine/kernel.h>

#include <game/version.h>
#include <game/generated/protocol_tdtw.h>

#include <tdtwsrv/game.h>
#include <tdtwsrv/tdtwsrv.h>

class CTdtwSrv : public ITDTWSrv
{
public:
	CTdtwSrv();
	~CTdtwSrv();

private:
	class IEngine *m_pEngine;
	class IStorage *m_pStorage;
	class IConfig *m_pConfig;
	class IConsole *m_pConsole;
	class IAutoUpdate *m_pAutoUpdate;
	class IGame *m_pGame;
public:
	class IStorage *Storage() { return m_pStorage; }
	class IConfig *Config() { return m_pConfig; }
	class IConsole *Console() { return m_pConsole; }
	class IGame *Game() { return m_pGame; }
	class IAutoUpdate *AutoUpdate() { return m_pAutoUpdate; }

	virtual void RequestInterfaces();

	int MaxClients() const { return m_NetServer.MaxClients(); }

	virtual int SendMsg(CMsgPacker *pMsg, int Flags, int ClientID);
	virtual int SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System);

	enum
	{
		AUTHED_NO = 0,
		AUTHED_MOD,
		AUTHED_ADMIN,

		MAX_RCONCMD_SEND = 16,
	};


	virtual void Run();
	void Protocol(CNetChunk *Chunk);

	static int NewClientCallback(int ClientID, void *pUser);
	static int DelClientCallback(int ClientID, const char *pReason, void *pUser);


	virtual const char *LatestVersion() { return GAME_VERSION; }
private:
	CNetTdtwServer m_NetServer;
	CNetObjHandlerTdtw m_NetHandler;	
};
#endif