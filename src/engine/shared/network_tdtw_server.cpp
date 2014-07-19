/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>

#include <engine/console.h>

#include "netban.h"
#include "network.h"


bool CNetTdtwServer::Open(NETADDR BindAddr, CNetBan *pNetBan, int MaxClients, int MaxClientsPerIP, int Flags)
{
	// zero out the whole structure
	mem_zero(this, sizeof(*this));

	// open socket
	m_Socket = net_udp_create(BindAddr);
	if(!m_Socket.type)
		return false;

	m_pNetBan = pNetBan;

	// clamp clients
	m_MaxClients = MaxClients;

	if(m_MaxClients < 1)
		m_MaxClients = 1;

	m_MaxClientsPerIP = MaxClientsPerIP;

	m_aSlots.clear();

	return true;
}

int CNetTdtwServer::SetCallbacks(NETFUNC_NEWCLIENT pfnNewClient, NETFUNC_DELCLIENT pfnDelClient, void *pUser)
{
	m_pfnNewClient = pfnNewClient;
	m_pfnDelClient = pfnDelClient;
	m_UserPtr = pUser;
	return 0;
}

int CNetTdtwServer::Close()
{
	// TODO: implement me
	return 0;
}

int CNetTdtwServer::Drop(int ClientID, const char *pReason)
{
	if(m_pfnDelClient)
		m_pfnDelClient(ClientID, pReason, m_UserPtr);

	m_aSlots[ClientID]->m_Connection.Disconnect(pReason);
	m_aSlots.remove_index(ClientID);

	return 0;
}

int CNetTdtwServer::Update()
{
	int64 Now = time_get();
	for(int i = 0; i < m_aSlots.size(); i++)
	{
		m_aSlots[i]->m_Connection.Update();
		if(m_aSlots[i]->m_Connection.State() == NET_CONNSTATE_ERROR)
		{
			if(Now - m_aSlots[i]->m_Connection.ConnectTime() < time_freq() && NetBan())
				NetBan()->BanAddr(ClientAddr(i), 60, "Stressing network");
			else
				Drop(i, m_aSlots[i]->m_Connection.ErrorString());
		}
	}

	return 0;
}

/*
	TODO: chopp up this function into smaller working parts
*/
int CNetTdtwServer::Recv(CNetChunk *pChunk)
{
	while(1)
	{
		NETADDR Addr;

		// check for a chunk
		if(m_RecvUnpacker.FetchChunk(pChunk))
			return 1;

		// TODO: empty the recvinfo
		int Bytes = net_udp_recv(m_Socket, &Addr, m_RecvUnpacker.m_aBuffer, NET_MAX_PACKETSIZE);

		// no more packets for now
		if(Bytes <= 0)
			break;

		if(CNetBase::UnpackPacket(m_RecvUnpacker.m_aBuffer, Bytes, &m_RecvUnpacker.m_Data) == 0)
		{
			// check if we just should drop the packet
			char aBuf[128];
			if(NetBan() && NetBan()->IsBanned(&Addr, aBuf, sizeof(aBuf)))
			{
				// banned, reply with a message
				CNetBase::SendControlMsg(m_Socket, &Addr, 0, NET_CTRLMSG_CLOSE, aBuf, str_length(aBuf)+1);
				continue;
			}

			if(m_RecvUnpacker.m_Data.m_Flags&NET_PACKETFLAG_CONNLESS)
			{
				pChunk->m_Flags = NETSENDFLAG_CONNLESS;
				pChunk->m_ClientID = -1;
				pChunk->m_Address = Addr;
				pChunk->m_DataSize = m_RecvUnpacker.m_Data.m_DataSize;
				pChunk->m_pData = m_RecvUnpacker.m_Data.m_aChunkData;
				return 1;
			}
			else
			{
				// TODO: check size here
				if(m_RecvUnpacker.m_Data.m_Flags&NET_PACKETFLAG_CONTROL && m_RecvUnpacker.m_Data.m_aChunkData[0] == NET_CTRLMSG_CONNECT)
				{
					bool Found = false;

					// check if we already got this client
					for (int i = 0; i < m_aSlots.size(); i++)
					{
						if(m_aSlots[i]->m_Connection.State() != NET_CONNSTATE_OFFLINE &&
							net_addr_comp(m_aSlots[i]->m_Connection.PeerAddress(), &Addr) == 0)
						{
							Found = true; // silent ignore.. we got this client already
							break;
						}
					}

					// client that wants to connect
					if(!Found)
					{
						// only allow a specific number of players with the same ip
						NETADDR ThisAddr = Addr, OtherAddr;
						int FoundAddr = 1;
						ThisAddr.port = 0;
						for (int i = 0; i < m_aSlots.size(); ++i)
						{
							if(m_aSlots[i]->m_Connection.State() == NET_CONNSTATE_OFFLINE)
								continue;

							OtherAddr = *m_aSlots[i]->m_Connection.PeerAddress();
							OtherAddr.port = 0;
							if(!net_addr_comp(&ThisAddr, &OtherAddr))
							{
								if(FoundAddr++ >= m_MaxClientsPerIP)
								{
									char aBuf[128];
									str_format(aBuf, sizeof(aBuf), "Only %d players with the same IP are allowed", m_MaxClientsPerIP);
									CNetBase::SendControlMsg(m_Socket, &Addr, 0, NET_CTRLMSG_CLOSE, aBuf, sizeof(aBuf));
									return 0;
								}
							}
						}

						if (m_aSlots.size() < MaxClients())
						{
							CSlot *NewSlot = new CSlot;
							NewSlot->m_Connection.Init(m_Socket, true);
							NewSlot->m_Connection.Feed(&m_RecvUnpacker.m_Data, &Addr);

							if (m_pfnNewClient)
								m_pfnNewClient(m_aSlots.add(NewSlot), m_UserPtr);
						}
						else
						{
							const char FullMsg[] = "This server is full";
							CNetBase::SendControlMsg(m_Socket, &Addr, 0, NET_CTRLMSG_CLOSE, FullMsg, sizeof(FullMsg));
						}
					}
				}
				else
				{
					// normal packet, find matching slot
					for (int i = 0; i < m_aSlots.size(); i++)
					{
						if(net_addr_comp(m_aSlots[i]->m_Connection.PeerAddress(), &Addr) == 0)
						{
							if(m_aSlots[i]->m_Connection.Feed(&m_RecvUnpacker.m_Data, &Addr))
							{
								if(m_RecvUnpacker.m_Data.m_DataSize)
									m_RecvUnpacker.Start(&Addr, &m_aSlots[i]->m_Connection, i);
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

int CNetTdtwServer::Send(CNetChunk *pChunk)
{
	if(pChunk->m_DataSize >= NET_MAX_PAYLOAD)
	{
		dbg_msg("netserver", "packet payload too big. %d. dropping packet", pChunk->m_DataSize);
		return -1;
	}

	if(pChunk->m_Flags&NETSENDFLAG_CONNLESS)
	{
		// send connectionless packet
		CNetBase::SendPacketConnless(m_Socket, &pChunk->m_Address, pChunk->m_pData, pChunk->m_DataSize);
	}
	else
	{
		int Flags = 0;
		dbg_assert(pChunk->m_ClientID >= 0, "errornous client id");
		dbg_assert(pChunk->m_ClientID < m_aSlots.size(), "errornous client id");

		if(pChunk->m_Flags&NETSENDFLAG_VITAL)
			Flags = NET_CHUNKFLAG_VITAL;

		if(m_aSlots[pChunk->m_ClientID]->m_Connection.QueueChunk(Flags, pChunk->m_DataSize, pChunk->m_pData) == 0)
		{
			if(pChunk->m_Flags&NETSENDFLAG_FLUSH)
				m_aSlots[pChunk->m_ClientID]->m_Connection.Flush();
		}
		else
		{
			Drop(pChunk->m_ClientID, "Error sending data");
		}
	}
	return 0;
}

void CNetTdtwServer::SetMaxClientsPerIP(int Max)
{
	// clamp
	if(Max < 1)
		Max = 1;

	m_MaxClientsPerIP = Max;
}
