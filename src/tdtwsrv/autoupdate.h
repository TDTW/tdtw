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
	struct FileInfo
	{
		char Name[128];
		int Crc;
		int Size;
	};
	struct DirInfo
	{
		char Name[128];
		int Crc;
		int Size;
		array <FileInfo> m_aFiles;
	};
	void CheckHash();

	class CTdtwSrv *Server() { return m_pServer; }
	class IStorage *Storage() { return m_pStorage; }
	static int ParseFilesCallback(const char *pFileName, int IsDir, int Type, void *pUser);
	static int ParseFolderCallback(const char *pFileName, int IsDir, int Type, void *pUser);
private:
	array <FileInfo> m_aFiles;
	array <DirInfo> m_aDir;
	class CTdtwSrv *m_pServer;
	class IStorage *m_pStorage;
};

#endif