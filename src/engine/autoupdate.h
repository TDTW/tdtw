/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H
#include "kernel.h"
#include "message.h"

class IAutoUpdate : public IInterface
{
	MACRO_INTERFACE("autoupdate", 0)
public:
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

	array <CInfoFolders> m_aDir;
	virtual void CheckHash() = 0;
	virtual void RequestInterfaces() = 0;
};

extern IAutoUpdate *CreateAutoUpdate();
#endif
