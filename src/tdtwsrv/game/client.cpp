#include "client.h"
#include <math.h>
#include "game.h"

CClientTdtw::CClientTdtw(CGame *Game, int ID)
{
	m_State = STATE_CONNECTED;
	m_ID = ID;

	m_FileData = 0;
	m_FileSize = 0;
	m_FileCRC = 0;
	m_FileChunks = 0;
	m_FileCurChunk = 0;
	m_pGame = Game;
	m_Updating = false;
}

CClientTdtw::~CClientTdtw()
{

}

bool CClientTdtw::OpenFile(const char *FileName)
{
	IOHANDLE File = io_open(FileName, IOFLAG_READ);

	if (!File)
		return false;
	
	m_FileSize = (int)io_length(File);

	if (m_FileData)
		mem_free(m_FileData);

	m_FileData = (unsigned char *)mem_alloc(m_FileSize, 1);
	m_FileChunks = ceil(m_FileSize / (1024.0f - 128.0f));

	dbg_msg("Chunks", "%d", m_FileChunks);

	io_read(File, m_FileData, m_FileSize);
	io_close(File);

	CMsgPacker Msg(NETMSG_TDTW_UPDATE_INFO);
	Msg.AddString(FileName, -1);
	Msg.AddInt(0);
	Msg.AddInt(m_FileSize);
	Msg.AddInt(m_FileChunks);
	Game()->Server()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, m_ID, true);

	return true;
}

void CClientTdtw::EndUpdate()
{
	m_FileData = 0;
	m_FileSize = 0;
	m_FileCRC = 0;
	m_FileChunks = 0;
	m_FileCurChunk = 0;
}

void CClientTdtw::GetHash()
{
	CMsgPacker Msg(NETMSG_TDTW_HASH_REQUEST);
	Msg.AddString("1", -1);
	Game()->Server()->SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, m_ID, true);
}