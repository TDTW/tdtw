#include "client.h"
#include <math.h>
#include <game\generated\protocol_tdtw.h>
#include "game.h"

CClientTdtw::CClientTdtw(CGame *Game, int ID)
{
	m_State = STATE_CONNECTED;
	m_GameState = 0;
	m_Latency = 0;
	m_ID = ID;
	m_UseCustomColor = 0;
	m_ColorBody = 0;
	m_ColorFeet = 0;
	m_SkinID = 0;
	m_SkinColor = 0;
	mem_zero(&m_aSkinName, sizeof(m_aSkinName));

	mem_zero(&m_aName, sizeof(m_aName));
	mem_zero(&m_aClan, sizeof(m_aClan));
	m_Country = 0;
	m_Score = 0;

	m_FileData = 0;
	m_FileSize = 0;
	m_FileCRC = 0;
	m_FileChunks = 0;
	m_FileCurChunk = 0;
	mem_zero(&ServerIP, sizeof(ServerIP));

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

	m_Authed = 0;
	m_AuthTries = 0;

	m_Followers.clear();
	mem_zero(&m_Following, sizeof(m_Following));

	m_Friends.clear();
}

CClientTdtw::~CClientTdtw()
{
	m_Followers.delete_all();
	m_Friends.delete_all();
	delete m_Following;
	delete m_pRconCmdToSend;
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
	CNetMsg_AutoUpdate_Hash Hash;
	Game()->Server()->SendPackMsg(&Hash, MSGFLAG_FLUSH | MSGFLAG_VITAL, m_ID, false);
}
