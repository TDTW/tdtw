#ifndef GAME_CLIENT_TDTWSERVER_TDTWSERVER_H
#define GAME_CLIENT_TDTWSERVER_TDTWSERVER_H
#include <base/system.h>
#include <engine/tdtwserver.h>
#include <engine/shared/network.h>
#include <engine/shared/packer.h>
#include <engine/kernel.h>
#include <engine/client.h>
#include <engine/console.h>
#include <engine/autoupdate.h>
#include <engine/shared/protocol.h>
#include <game/generated/protocol_tdtw.h>
#include <game/version.h>
#include "updatefiles.h"

class CTDTWServer : public ITDTWServer 
{
public:
	CTDTWServer();
	virtual void Recv(CNetChunk *pChunk);
	virtual void Init(); 
	void Protocol(CNetChunk *pChunk);

	class IClient *Client() { return m_pClient; }
	class IConsole *Console() { return m_pConsole; }
	class IAutoUpdate *AutoUpdate() { return m_pAutoUpdate; }
	IKernel *Kernel() { return IInterface::Kernel(); }
private:
	CNetObjHandlerTdtw m_NetHandler;
	char *m_Version;

	int m_FileCrc;
	int m_FileChunk;
	int m_FileDownloadAmount;
	int m_FileTotalChunks;
	int m_FileTotalSize;
	unsigned char *m_FileData[10];
	int m_FileChunkSize[10];
	IOHANDLE m_FileHandle;

	class CUpdateFiles *UpdateFiles;

	class IClient *m_pClient;
	class IConsole *m_pConsole;
	class IAutoUpdate *m_pAutoUpdate;
};

#endif //GAME_CLIENT_TDTWSERVER_TDTWSERVER_H