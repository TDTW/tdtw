#ifndef TDTWSRV_AUTOUPDATE_H
#define TDTWSRV_AUTOUPDATE_H

#include <base\system.h>
#include <base\tl\array.h>

#include <engine\shared\datafile.h>
#include <engine\kernel.h>
#include <engine\storage.h>
#include "tdtwsrv.h"

class CAutoUpdate
{	
public:
	CAutoUpdate(class CTdtwSrv *Server, IStorage *Storage);
	
	struct CInfo
	{
		char Name[128];
		int Crc;
		int Size;
		int ID;
		int ParentID;
	};
	void CheckHash();

	class CTdtwSrv *Server() { return m_pServer; }
	class IStorage *Storage() { return m_pStorage; }
	static int ParseFilesCallback(const char *pFileName, int IsDir, void *pUser, int folder_id);
private:

	array <CInfo> m_aDir;
	class CTdtwSrv *m_pServer;
	class IStorage *m_pStorage;
	int TempID;
};

#endif