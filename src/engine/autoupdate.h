/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_AUTOUPDATE_H
#define ENGINE_AUTOUPDATE_H
#include "kernel.h"
#include "message.h"
#include <base\tl\array.h>

class IAutoUpdate : public IInterface
{
	MACRO_INTERFACE("autoupdate", 0)
public:
	struct CInfoFiles
	{
		char Name[128];
		int Crc;
		int Size;
		int ParentFolderID;
		bool IsFolder;
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
	virtual void SetNeedReplace(bool Replace) = 0;
	virtual bool GetNeedReplace() = 0;
	array <CInfoFolders> m_aDir;
	virtual void CheckHash() = 0;
	virtual void RequestInterfaces() = 0;
	virtual bool CheckVersion(char *Version) = 0;
	virtual void ReplaceFileUpdate(char *FileName) = 0;
};

extern IAutoUpdate *CreateAutoUpdate();
#endif
