#include "game.h"

CGame::CGame()
{
	m_apClients.clear();
}

int CGame::ClientState(int ClientID)
{
	return m_apClients[ClientID]->m_State;
}

void CGame::AddClient()
{
	CClientTdtw *NewClient = new CClientTdtw;
	m_apClients.add(NewClient);
}

void CGame::DeleteClient(int ClientID)
{
	m_apClients.remove_index(ClientID);
}

int CGame::ClientsNum()
{
	return m_apClients.size();
}


IGame *CreateGame() { return new CGame; }