#include <stdlib.h>
#include <time.h>
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
    if(str_comp(pFile, "teeworlds.exe"))
    {
        char aBuf[512];
        srand (time(NULL));

        /* generate secret number between 1 and 10: */
        int Num = rand() % 100000;
        str_format(aBuf, sizeof(aBuf), "%s_%d", pFile, Num);
        m_Server->AutoUpdate()->SetBinaryTempNumber(Num);
        str_copy(File.Name, aBuf, sizeof(File.Name));
    }
    else
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