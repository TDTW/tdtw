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
	m_aDir.clear();
	File = io_open("Server.log", IOFLAG_WRITE);
	TempID = 0;
}
int CAutoUpdate::ParseFilesCallback(const char *pFileName, int IsDir, void *pUser, int folder_id)
{
	const int PathLength = str_length(pFileName);
	if (pFileName[0] == '.')
		return 0;

	CAutoUpdate *pThis = (CAutoUpdate *)pUser;

	if (!IsDir)
	{
		CInfo *Temp = new CInfo();
		str_copy(Temp->Name, pFileName, sizeof(Temp->Name));
		Temp->ID = pThis->TempID;
		Temp->ParentID = folder_id;
		CDataFileReader::GetCrcSize(pThis->Storage(), Temp->Name, IStorage::TYPE_ALL, (unsigned int *)&Temp->Crc, (unsigned int *)&Temp->Size);
		pThis->m_aDir.add(*Temp);
		pThis->TempID++;
		
		char parent_name[128];
		for (int i = 0; i < pThis->m_aDir.size(); i++)
			if (pThis->m_aDir[i].ID == folder_id)
				str_copy(parent_name, pThis->m_aDir[i].Name, sizeof(parent_name));

		char abuf[128];
		str_format(abuf, sizeof(abuf), "----[%d]%s  Name: %s    CRC: %d", Temp->ID ,parent_name, Temp->Name, Temp->Crc);
		io_write(pThis->File, abuf, str_length(abuf));
		io_write_newline(pThis->File);
	}
	else
	{		
		char aBuf[128];
		str_copy(aBuf, pFileName, sizeof(aBuf));
		for (int i = 0; i < pThis->m_aDir.size(); i++)
			if (pThis->m_aDir[i].ID == folder_id)
				str_format(aBuf, sizeof(aBuf), "update/%s/%s", pThis->m_aDir[i].Name, pFileName);

		CInfo *Temp = new CInfo();
		str_copy(Temp->Name, aBuf, sizeof(Temp->Name));
		Temp->ID = pThis->TempID;
		Temp->ParentID = folder_id;
		pThis->m_aDir.add(*Temp);

		char abuf[128];
		str_format(abuf, sizeof(abuf), "[DIR] %s", Temp->Name);
		io_write(pThis->File, abuf, str_length(abuf));
		io_write_newline(pThis->File);
		pThis->TempID++;
		fs_listdir2(Temp->Name, ParseFilesCallback, pThis, Temp->ID);
	}
	return 0;
}

void CAutoUpdate::CheckHash()
{
	fs_listdir2("update", ParseFilesCallback, this, -1);
	io_close(File);
}
