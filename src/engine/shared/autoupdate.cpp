#include "autoupdate.h"

CAutoUpdate::CAutoUpdate()
{
	m_aDir.clear();

	CInfoFolders *Temp = new CInfoFolders();
	str_copy(Temp->Name, ".", sizeof(Temp->Name));

	Temp->FolderID = 0;
	Temp->ParentFolderID = -1;
	m_aDir.add(*Temp);	
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

			CDataFileReader::GetCrcSize(pThis->Storage(), Temp->Name, IStorage::TYPE_ALL, (unsigned int *)&Temp->Crc, (unsigned int *)&Temp->Size);

			Temp->FolderID = folder_id;
			pThis->m_aDir[folder_id].m_aFiles.add(*Temp);

			pThis->m_aDir[folder_id].Crc += Temp->Crc;
		}
		else
		{
			CInfoFolders *Temp = new CInfoFolders();
			str_copy(Temp->Name, aBuf, sizeof(Temp->Name));

			Temp->FolderID = pThis->m_aDir.size();
			Temp->ParentFolderID = folder_id;
			pThis->m_aDir.add(*Temp);
			
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
			Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "updater", "   [%d] [%08x] %s", m_aDir[i].m_aFiles[j].FolderID, m_aDir[i].m_aFiles[j].Crc, m_aDir[i].m_aFiles[j].Name);
		}
	}*/
}

bool CAutoUpdate::CheckVersion(char *Version)
{
	if (str_comp(GAME_VERSION, Version) == 0)
		return true;
	return false;
}

void CAutoUpdate::RequestInterfaces()
{
	m_pConsole = Kernel()->RequestInterface<IConsole>();
	m_pStorage = Kernel()->RequestInterface<IStorage>();
}

IAutoUpdate *CreateAutoUpdate() { return new CAutoUpdate(); }