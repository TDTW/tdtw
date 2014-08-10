#ifndef TDTWSRV_GAME_H
#define TDTWSRV_GAME_H
#include <engine\kernel.h>
#include <engine\message.h>
#include "game\client.h"
class IGame : public IInterface
{
	MACRO_INTERFACE("game", 0)
public:
	virtual int ClientState(int ClientID) = 0;
	virtual void AddClient() = 0;
	virtual void DeleteClient(int ClientID) = 0;
	virtual int ClientsNum() = 0;

	array <CClientTdtw *> m_apClients;
};

extern IGame *CreateGame();
#endif