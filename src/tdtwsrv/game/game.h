#ifndef TDTWSRV_GAME_GAME_H
#define TDTWSRV_GAME_GAME_H
#include "client.h"
#include <tdtwsrv\game.h>
class CGame : public IGame
{
public:
	CGame();
	virtual int ClientState(int ClientID);
	virtual void AddClient();
	virtual void DeleteClient(int ClientID);
	virtual int ClientsNum();

};
#endif