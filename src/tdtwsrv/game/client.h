#ifndef TDTWSRV_TDTWSRV_CLIENT_H
#define TDTWSRV_TDTWSRV_CLIENT_H

#include <engine\shared\protocol.h>
#include <engine\console.h>
#include <base\tl\array.h>

class CClientTdtw
{
public:
	CClientTdtw(class CGame *Game, int ID);
	~CClientTdtw();
	void GetHash();
	bool OpenFile(const char *FileName);

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

	int m_FileSize;
	int m_FileCRC;
	int m_FileChunks;
	int m_FileCurChunk;
	unsigned char *m_FileData;

	array <CClientTdtw*> m_Followers;
	CClientTdtw *m_Following;

	array <CClientTdtw*> m_Friends;

	const IConsole::CCommandInfo *m_pRconCmdToSend;
	class CGame *Game() { return m_pGame; }
private:
	int m_ID;
	class CGame *m_pGame;
};
#endif