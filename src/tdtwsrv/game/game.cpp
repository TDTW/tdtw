#include "game.h"

CGame::CGame(ITDTWSrv *Server)
{
	m_apClients.clear();
	m_pServer = Server;
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


IGame *CreateGame(ITDTWSrv *Server) { return new CGame(Server); }