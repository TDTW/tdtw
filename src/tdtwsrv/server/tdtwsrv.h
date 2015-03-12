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
#include "protocol.h"

class CTdtwSrv : public ITDTWSrv
{
public:
	CTdtwSrv();
	~CTdtwSrv();

	virtual void RequestInterfaces();

	//int MaxClients() const { return m_NetServer.MaxClients(); }

	virtual int SendMsg(CMsgPacker *pMsg, int Flags, int ClientID);
	virtual int SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System);

	virtual void Run();
	void Protocol(CNetChunk *Chunk);

	static int NewClientCallback(int ClientID, void *pUser);
	static int DelClientCallback(int ClientID, const char *pReason, void *pUser);

    CProtocol *Protocol() { return m_pProtocol; }
private:
    CProtocol *m_pProtocol;
	CNetTdtwServer m_NetServer;
};
#endif