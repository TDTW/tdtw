#include "client.h"

CClientTdtw::CClientTdtw()
{
	m_State = STATE_CONNECTED;
	m_GameState = 0;
	m_Latency = 0;

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

	mem_zero(&ServerIP, sizeof(ServerIP));

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
