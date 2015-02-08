#include "game.h"
#include "client.h"

CGame::CGame()
{
	m_apClients.clear();
}

int CGame::ClientState(int ClientID)
{
	return m_apClients[ClientID]->m_State;
}

void CGame::AddClient(int ID)
{
	CClientTdtw *NewClient = new CClientTdtw(this, ID);
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

void CGame::RequestInterfaces()
{
	m_pServer = Kernel()->RequestInterface<ITDTWSrv>();
}
IGame *CreateGame() { return new CGame; }