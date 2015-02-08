#include "updatefiles.h"

CUpdateFiles::CUpdateFiles(CTDTWServer *Server)
{
	m_Server = Server;
	m_UpdateFiles.clear();
	m_Updating = false;
}

void CUpdateFiles::AddFile(const char *pFile)
{
	CInfoFiles File;
	str_copy(File.Name, pFile, sizeof(File.Name));
	m_UpdateFiles.add(File);
}

void CUpdateFiles::StartUpdate()
{
	if (m_UpdateFiles.size() == 0 || m_Updating)
		return;
	CMsgPacker Msg(NETMSG_TDTW_UPDATE_INFO);
	Msg.AddString(m_UpdateFiles[0].Name, -1);
	Server()->Client()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, true, true);
	m_Updating = true;
}

void CUpdateFiles::EndUpdate()
{	
	m_UpdateFiles.remove_index(0);
	m_Updating = false;
	StartUpdate();
}