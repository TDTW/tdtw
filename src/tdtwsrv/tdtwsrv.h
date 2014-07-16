
#ifndef TDTWSRV_H
#define TDTWSRV_H

#include <base\system.h>

#include <engine\shared\network.h>

#define TDTW_IP "127.0.0.1:8000"
#define TDTW_PORT 8000

class TdtwSrv
{
public:
	TdtwSrv();
	~TdtwSrv();

	int Run();
	void Protocol(CNetChunk *Chunk);
private:
	CNetServer pNet;

};
#endif