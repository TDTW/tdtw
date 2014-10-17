#ifndef TDTWSRV_GAME_GAME_H
#define TDTWSRV_GAME_GAME_H
#include "client.h"
#include <tdtwsrv\game.h>
#include <tdtwsrv\tdtwsrv.h>
class CGame : public IGame
{
public:
	CGame();
	virtual int ClientState(int ClientID);
	virtual void AddClient(int ID);
	virtual void DeleteClient(int ClientID);
	virtual int ClientsNum();
	virtual void AddUpdateFile(int ID, char *Name, int CRC);
	virtual void RequestInterfaces();
	ITDTWSrv *Server() { return m_pServer; }
	
private:
	class ITDTWSrv *m_pServer;
};
#endif