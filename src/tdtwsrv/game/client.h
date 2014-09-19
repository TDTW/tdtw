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

	IAutoUpdate::CInfoFolders m_Folders;

	int m_FileSize;
	int m_FileCRC;
	int m_FileChunks;
	int m_FileCurChunk;
	unsigned char *m_FileData;

/*	array <CClientTdtw*> m_Followers;
	CClientTdtw *m_Following;
	array <CClientTdtw*> m_Friends;*/
	
	void StartUpdate();

	class CGame *Game() { return m_pGame; }
private:
	int m_ID;
	class CGame *m_pGame;
};
#endif