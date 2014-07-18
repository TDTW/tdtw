
#ifndef TDTWSRV_H
#define TDTWSRV_H

#include <base\system.h>

#include <engine\shared\network.h>
#include <engine\shared\netban.h>

#include <engine\console.h>
#include <engine\storage.h>
#include <engine\config.h>

#include <game\generated\protocol_tdtw.h>

class TdtwSrv
{
public:
	TdtwSrv(int argc, const char **argv);
	~TdtwSrv();

	int Run();
	void Protocol(CNetChunk *Chunk);

private:
	CNetServer pNet;
	CNetObjHandlerTdtw m_NetHandler;

	IKernel *pKernel;
	IStorage *pStorage;
	IConfig *pConfig;
	IConsole *m_pConsole;

	CNetBan m_NetBan;

	int64 LastBanReload = 0;

	void ReloadBans();
};
#endif