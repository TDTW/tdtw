#include "autoupdate.h"
#include <base/system.h>
/*
MINI TUTORIAL HAW WARK THES ABDUTER BAI MIXA POBEDIATEL EVROVIDENYA 2008
	server->client | srv_ver
	client->server | cli_ver
	------------------ -
	client->server | hash
	teeworlds.exe
	sdl.dll
	freetype.dll
	/ data / audio
	/ data / countryflags
	/ data / editor
	/ data / fonts
	/ data / languages
	/ data / mapres
	/ data / maps
	/ data / skins
	/ data

	server.exe

	teeworlds.exe
	sdl.dll
	freetype.dll
	server->client | detail hash request
	/ data / skins

	teeworlds.exe
	client->server | hash list

	CDataFileReader::GetCrcSize()    ---------  CRC and Size file;

*/

CAutoUpdate::CAutoUpdate(class CTdtwSrv *Server, IStorage *Storage)
{
	m_pServer = Server;
	m_pStorage = Storage;
	{
		m_aDir.clear();

		CInfoFolders *Temp = new CInfoFolders();
		str_copy(Temp->Name, ".", sizeof(Temp->Name));

		Temp->FolderID = 0;
		Temp->ParentFolderID = -1;
		m_aDir.add(*Temp);
	}
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

/*	// Logger of updated files with CRC and folders
	for (int i = 0; i < m_aDir.size(); i++)
	{
		Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "updater", "[%d][%d] [%08x] %s", m_aDir[i].ParentFolderID, m_aDir[i].FolderID, m_aDir[i].Crc, m_aDir[i].Name);
		for (int j = 0; j < m_aDir[i].m_aFiles.size(); j++)
		{
			Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "updater", "   [%d] [%08x] %s", m_aDir[i].m_aFiles[j].FolderID, m_aDir[i].m_aFiles[j].Crc, m_aDir[i].m_aFiles[j].Name);
		}
	}*/
}
