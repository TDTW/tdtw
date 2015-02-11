#include <base/system.h>
#include "autoupdate.h"
#include "datafile.h"

#if defined(CONF_FAMILY_WINDOWS)
#define IStorage _IStorage
#include <windows.h>
#undef IStorage
#endif
#include <game/version.h>
#include <engine/autoupdate.h>
#include <time.h>

CAutoUpdate::CAutoUpdate()
{
	m_aDir.clear();

	CInfoFolders *Temp = new CInfoFolders();
	str_copy(Temp->Name, ".", sizeof(Temp->Name));

	Temp->FolderID = 0;
	Temp->ParentFolderID = -1;
	m_aDir.add(*Temp);	
	NeedReplace = false;
    SecretNum = 0;
}

void CAutoUpdate::CheckHash()
{
	static FS_LISTDIR_CALLBACK2 ParseFilesCallback = [](const char *pFileName, int IsDir, void *pUser, int folder_id)
	{
		const int PathLength = str_length(pFileName);
		if (pFileName[0] == '.')
			return 0;

		CAutoUpdate *pThis = (CAutoUpdate *)pUser;

		char aBuf[128];
		str_copy(aBuf, pFileName, sizeof(aBuf));
		for (int i = 0; i < pThis->m_aDir.size(); i++)
			if (pThis->m_aDir[i].FolderID == folder_id)
				str_format(aBuf, sizeof(aBuf), "%s/%s", pThis->m_aDir[i].Name, pFileName);

		if (!IsDir)
		{
			CInfoFiles *Temp = new CInfoFiles();
			str_copy(Temp->Name, aBuf, sizeof(Temp->Name));

			CDataFileReader::GetHashSize(pThis->Storage(), Temp->Name, -1, (unsigned int *)&Temp->Hash, (unsigned int *)&Temp->Size);

			Temp->ParentFolderID = folder_id;
			Temp->IsFolder = false;
			Temp->FolderID = 0;
			pThis->m_aDir[folder_id].m_aFiles.add(*Temp);

			pThis->m_aDir[folder_id].Hash += Temp->Hash;
		}
		else
		{
			CInfoFolders *Temp = new CInfoFolders();
			str_copy(Temp->Name, aBuf, sizeof(Temp->Name));

			Temp->FolderID = pThis->m_aDir.size();
			Temp->ParentFolderID = folder_id;
			pThis->m_aDir.add(*Temp);

			CInfoFiles* Temp2 = new CInfoFiles();
			str_copy(Temp2->Name, aBuf, sizeof(Temp2->Name));
			Temp2->ParentFolderID = folder_id;
			Temp2->IsFolder = true;
			Temp2->FolderID = pThis->m_aDir.size()-1;
			
			pThis->m_aDir[folder_id].m_aFiles.add(*Temp2);
			
			fs_listdir2(Temp->Name, ParseFilesCallback, pThis, Temp->FolderID);
		}
		return 0;
	};
	fs_listdir2("./", ParseFilesCallback, this, 0);

	// Logger of updated files with CRC and folders
	/*for (int i = 0; i < m_aDir.size(); i++)
	{
		Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "updater", "[%d][%d] [%08x] %s", m_aDir[i].ParentFolderID, m_aDir[i].FolderID, m_aDir[i].Crc, m_aDir[i].Name);
		for (int j = 0; j < m_aDir[i].m_aFiles.size(); j++)
		{
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "updater", "   [%d] [%08x] %s", m_aDir[i].m_aFiles[j].ParentFolderID, m_aDir[i].m_aFiles[j].Crc, m_aDir[i].m_aFiles[j].Name);
		}
	}*/
}

bool CAutoUpdate::CheckVersion(char *Version)
{
    return str_comp(GAME_VERSION, Version) == 0;

}

void CAutoUpdate::RequestInterfaces()
{
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	m_pStorage = Kernel()->RequestInterface<IStorage>();
}

void CAutoUpdate::ReplaceFileUpdate(char *FileName)
{
	char aBuf[512];
	str_format(aBuf, sizeof(aBuf), ":_R\r\ndel \"%s\"\r\n"
            "if exist \"%s\" goto _R\r\nrename \"teeworlds_%d.exe\" \"%s\"\r\n"
            ":_T\r\nif not exist \"%s\" goto _T\r\n"
            /*"del \"replace.bat\"\r\n"*/, FileName, FileName, SecretNum, FileName, FileName);
	IOHANDLE File = io_open("replace.bat", IOFLAG_WRITE);
	io_write(File, aBuf, str_length(aBuf));
	io_close(File);
#if defined(CONF_FAMILY_WINDOWS)
	ShellExecuteA(0, 0, "replace.bat", 0, 0, SW_HIDE);
#elif defined(CONF_PLATFORM_LINUX)
    char aBuf[128];
    str_format(aBuf, sizeof(aBuf), "teeworlds_%d", SecretNum);
	if (rename(aBuf, FileName))
		dbg_msg("autoupdate", "Error renaming binary file");
    str_format(aBuf, sizeof(aBuf), "chmod +x %s", FileName);
	if (system(aBuf))
		dbg_msg("autoupdate", "Error setting executable bit");
#endif
}

IAutoUpdate *CreateAutoUpdate() { return new CAutoUpdate(); }
