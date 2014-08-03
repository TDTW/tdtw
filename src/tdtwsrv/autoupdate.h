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

	struct CInfoFiles
	{
		char Name[128];
		int Crc;
		int Size;
		int FolderID;
	};
	struct CInfoFolders 
	{
		char Name[128];
		int Crc;
		int Size;
		int FolderID;
		int ParentFolderID;
		array <CInfoFiles> m_aFiles;
	};
	void CheckHash();

	class CTdtwSrv *Server() { return m_pServer; }
	class IStorage *Storage() { return m_pStorage; }
private:

	array <CInfoFolders> m_aDir;
	class CTdtwSrv *m_pServer;
	class IStorage *m_pStorage;
};

#endif