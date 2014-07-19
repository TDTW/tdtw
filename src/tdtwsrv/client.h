
#ifndef TDTWSRV_CLIENT_H
#define TDTWSRV_CLIENT_H

#include <engine\shared\protocol.h>
#include <engine\console.h>
#include <base\tl\array.h>

class CClientTdtw
{
public:
	CClientTdtw();
	~CClientTdtw();

	enum
	{
		STATE_EMPTY = 0,
		STATE_CONNECTED
	};

	// connection state info
	int m_State;
	int m_GameState;
	int m_Latency;

	int m_UseCustomColor;
	int m_ColorBody;
	int m_ColorFeet;
	char m_aSkinName[64];
	int m_SkinID;
	int m_SkinColor;

	char m_aName[MAX_NAME_LENGTH];
	char m_aClan[MAX_CLAN_LENGTH];
	int m_Country;
	int m_Score;

	NETADDR ServerIP;
	
	int m_Authed;
	int m_AuthTries;

	array <CClientTdtw*> m_Followers;
	CClientTdtw *m_Following;

	array <CClientTdtw*> m_Friends;

	const IConsole::CCommandInfo *m_pRconCmdToSend;

};
#endif