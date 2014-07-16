#include "tdtwsrv.h"

#include <base\system.h>

#include <engine\shared\config.h>
#include <engine\shared\network.h>
#include <engine\kernel.h>
#include <engine\console.h>

#include <iostream>

TdtwSrv::TdtwSrv()
{
}

TdtwSrv::~TdtwSrv()
{
}

int TdtwSrv::Run()
{
	int64 NextHeartBeat = 0;

	net_init();

	// start server
	NETADDR BindAddr;
	if (net_host_lookup("127.0.0.1:8000", &BindAddr, NETTYPE_ALL) == 0)
	{
		// sweet!
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = 8000;
	}
	else
	{
		mem_zero(&BindAddr, sizeof(BindAddr));
		BindAddr.type = NETTYPE_ALL;
		BindAddr.port = 8000;
	}

	if (!pNet.Open(BindAddr, 0, 0, 0, 0))
		return 0;

	while (1)
	{
		CNetChunk p;
		pNet.Update();
		while (pNet.Recv(&p))
		{
			if (p.m_ClientID == -1)
			{
				Protocol(&p);
							
				std::cout << p.m_pData << std::endl;

/*				if (p.m_DataSize == sizeof(SERVERBROWSE_GETINFO) &&
					mem_comp(p.m_pData, SERVERBROWSE_GETINFO, sizeof(SERVERBROWSE_GETINFO)) == 0)
				{
					SendServerInfo(&p.m_Address);
				}
				else if (p.m_DataSize == sizeof(SERVERBROWSE_FWCHECK) &&
					mem_comp(p.m_pData, SERVERBROWSE_FWCHECK, sizeof(SERVERBROWSE_FWCHECK)) == 0)
				{
					SendFWCheckResponse(&p.m_Address);
				}*/
			}
		}

		thread_sleep(100);
	}
}

void TdtwSrv::Protocol(CNetChunk *Chunk)
{

}

int main(int argc, char **argv)
{
	TdtwSrv *TdtwServer = new TdtwSrv();
	return TdtwServer->Run();
}