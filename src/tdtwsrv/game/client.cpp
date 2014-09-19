#include "client.h"
#include <math.h>
#include <game\generated\protocol_tdtw.h>
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

	/*
	{
		IOHANDLE File = io_open("teeworlds_d.exe", IOFLAG_READ);
		m_FileSize = (int)io_length(File);
		if (m_FileData)
			mem_free(m_FileData);
		m_FileData = (unsigned char *)mem_alloc(m_FileSize, 1);
		m_FileChunks = ceil(m_FileSize/(1024.0f - 128.0f));
		dbg_msg("Chunks", "%d", m_FileChunks);
		io_read(File, m_FileData, m_FileSize);
		io_close(File);
	}*/
	m_pGame = Game;

/*	m_Followers.clear();
	mem_zero(&m_Following, sizeof(m_Following));

	m_Friends.clear();*/
}

CClientTdtw::~CClientTdtw()
{
/*	m_Followers.delete_all();
	m_Friends.delete_all();
	delete m_Following;
	delete m_pRconCmdToSend;*/
}

bool CClientTdtw::OpenFile(const char *FileName)
{
	IOHANDLE File = io_open("teeworlds_d.exe", IOFLAG_READ);
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
	return true;
}


void CClientTdtw::GetHash()
{
	CMsgPacker Msg(NETMSG_TDTW_HASH_REQUEST);
	Msg.AddString(".");
	SendMsgEx(&Msg, MSGFLAG_VITAL | MSGFLAG_FLUSH, m_ID, true);
}

void CClientTdtw::StartUpdate()
{
/*	for (int i = 0; i < UpdateFiles.size(); i++)
		dbg_msg("UpdtaeFiles", "Name:%s", UpdateFiles[i].Name);*/
}
