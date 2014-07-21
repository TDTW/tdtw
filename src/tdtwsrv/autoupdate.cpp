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
	m_aFiles.clear();
	m_aDir.clear();
}
int CAutoUpdate::ParseFilesCallback(const char *pFileName, int IsDir, int Type, void *pUser)
{
	const int PathLength = str_length(pFileName);
	if (str_comp(pFileName, ".") || str_comp(pFileName, ".."))
		return 0;
	CAutoUpdate *pThis = (CAutoUpdate *)pUser;
	if (!IsDir)
	{
		for (int i = 0; i < pThis->m_aFiles.size(); i++)
			if (!str_comp(pThis->m_aFiles[i].Name, pFileName))
				return 0;
		CAutoUpdate::FileInfo *temp = new CAutoUpdate::FileInfo();
		str_copy(temp->Name, pFileName, sizeof(temp->Name));
		CDataFileReader::GetCrcSize(pThis->Storage(), pFileName, IStorage::TYPE_ALL, (unsigned int *)&temp->Crc, (unsigned int *)&temp->Size);
		pThis->m_aFiles.add(*temp);
	}
	else
	{
		for (int i = 0; i < pThis.m_aDir.size(); i++)
			if (!str_comp(pThis->m_aDir[i].Name, pFileName))
				return 0;
		CAutoUpdate::DirInfo *temp = new CAutoUpdate::DirInfo();
		str_copy(temp->Name, pFileName, sizeof(temp->Name));
		CDataFileReader::GetCrcSize(pThis->Storage(), pFileName, IStorage::TYPE_ALL, (unsigned int *)&temp->Crc, (unsigned int *)&temp->Size);
		pThis->m_aDir.add(*temp);
		pThis->Storage()->ListDirectory(IStorage::TYPE_ALL, pFileName, ParseFilesCallback, pThis);
	}
	
	return 0;
}
int CAutoUpdate::ParseFolderCallback(const char *pFileName, int IsDir, int Type, void *pUser)
{
	const int PathLength = str_length(pFileName);
	if (str_comp(pFileName, ".") || str_comp(pFileName, ".."))
		return 0;
	CAutoUpdate *pThis = (CAutoUpdate *)pUser;
	if (!IsDir)
	{
		for (int i = 0; i < pThis->m_aFiles.size(); i++)
			if (!str_comp(pThis->m_aFiles[i].Name, pFileName))
				return 0;
		CAutoUpdate::FileInfo *temp = new CAutoUpdate::FileInfo();
		str_copy(temp->Name, pFileName, sizeof(temp->Name));
		CDataFileReader::GetCrcSize(pThis->Storage(), pFileName, IStorage::TYPE_ALL, (unsigned int *)&temp->Crc, (unsigned int *)&temp->Size);
		pThis->m_aFiles.add(*temp);
	}
	else
	{
		for (int i = 0; i < pThis.m_aDir.size(); i++)
			if (!str_comp(pThis->m_aDir[i].Name, pFileName))
				return 0;
		CAutoUpdate::DirInfo *temp = new CAutoUpdate::DirInfo();
		str_copy(temp->Name, pFileName, sizeof(temp->Name));
		CDataFileReader::GetCrcSize(pThis->Storage(), pFileName, IStorage::TYPE_ALL, (unsigned int *)&temp->Crc, (unsigned int *)&temp->Size);
		pThis->m_aDir.add(*temp);
		pThis->Storage()->ListDirectory(IStorage::TYPE_ALL, pFileName, ParseFilesCallback, pThis);
	}

	return 0;
}
void CAutoUpdate::CheckHash()
{
	Storage()->ListDirectory(IStorage::TYPE_ALL, "update", ParseFolderCallback, this);
	for (int i = 0; i < m_aFiles.size();  i++)
		Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "[%d] Name: %s : CRC: %d", i, m_aFiles[i].Name, m_aFiles[i].Crc);
}
