#include <base/system.h>
#include <engine/shared/protocol.h>
#include <engine/message.h>
#include <engine/console.h>
#include <tdtwsrv/game/client.h>
#include "protocol.h"

CProtocol::CProtocol(ITDTWSrv *pServer)
{
    m_pServer = pServer;
}
CProtocol::~CProtocol()
{
    delete m_pServer;
}

bool CProtocol::Protocol(CUnpacker *Unpacker, int Msg, int Sys, int ClientID)
{
    if (Sys)
    {
        if(!ParseSystemMsg(Unpacker, Msg, ClientID))
            return true;
    }
    else
    {
        if(!ParseMsg(Unpacker, Msg, ClientID))
            return true;
    }
    return false;
}

bool CProtocol::ParseSystemMsg(CUnpacker *Unpacker, int Msg, int ClientID)
{
    if (Msg == NETMSG_PING)
    {
        CMsgPacker msgPacker(NETMSG_PING_REPLY);
        Server()->SendMsgEx(&msgPacker, MSGFLAG_VITAL, ClientID, true);
        Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_STANDARD, "server", "[%d] NetPing", ClientID);
    }
    else if (Msg == NETMSG_TDTW_VERSION)
    {
        char *Version = (char *)Unpacker->GetString(CUnpacker::SANITIZE_CC);
        Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server", "[%d] Client version: %s", ClientID, Version);
        Server()->Game()->m_apClients[ClientID]->GetHash();
    }
    else if (Msg == NETMSG_TDTW_UPDATE_INFO)
    {
        char *File = (char *)Unpacker->GetString(CUnpacker::SANITIZE_CC);
        Server()->Game()->m_apClients[ClientID]->OpenFile(File);
    }
    else if (Msg == NETMSG_TDTW_UPDATE_REQUEST)
    {
        //int ChunkSize = 1024 - 128;
        int Offset = 0;
        for (int i = 0; i < 10; i++)
        {
            int ChunkSize;
            if (Server()->Game()->m_apClients[ClientID]->m_FileSize - (Server()->Game()->m_apClients[ClientID]->m_FileCurChunk*(1024 - 128)) >(1024 - 128))
                ChunkSize = 1024 - 128;
            else
                ChunkSize = Server()->Game()->m_apClients[ClientID]->m_FileSize - (Server()->Game()->m_apClients[ClientID]->m_FileCurChunk*(1024 - 128));

            if (Server()->Game()->m_apClients[ClientID]->m_FileChunks <= Server()->Game()->m_apClients[ClientID]->m_FileCurChunk)
            {
				char aBuf[256];
				str_format(aBuf, sizeof(aBuf), "sending file %s to client %d completed ", Server()->Game()->m_apClients[ClientID]->m_FileName ,ClientID);
				Server()-> Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "server", aBuf);
                Server()->Game()->m_apClients[ClientID]->EndUpdate();
                return true;
            }

            Offset = Server()->Game()->m_apClients[ClientID]->m_FileCurChunk * (1024 - 128);

            CMsgPacker msgPacker(NETMSG_TDTW_UPDATE_DATA);

            msgPacker.AddInt(Server()->Game()->m_apClients[ClientID]->m_FileCurChunk);
            msgPacker.AddInt(ChunkSize);
            msgPacker.AddRaw(&Server()->Game()->m_apClients[ClientID]->m_FileData[Offset], ChunkSize);
            Server()->SendMsgEx(&msgPacker, MSGFLAG_VITAL | MSGFLAG_FLUSH, ClientID, true);
            Server()->Game()->m_apClients[ClientID]->m_FileCurChunk++;
        }
    }
    else if (Msg == NETMSG_TDTW_HASH_REQUEST)
    {
        const char *pFile = Unpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
        if (str_comp(".", pFile))
        {
            for (int i = 0; i < Server()->AutoUpdate()->m_aDir.size(); i++)
            {
                if (str_comp(Server()->AutoUpdate()->m_aDir[i].Name, pFile) == 0)
                {
                    for (int j = 0; j < Server()->AutoUpdate()->m_aDir[i].m_aFiles.size(); j++)
                    {
                        CNetMsg_AutoUpdate_Hash updateHash;
                        updateHash.m_Name = Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].Name;

                        if (Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].IsFolder)
                            updateHash.m_Hash = Server()->AutoUpdate()->m_aDir[Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].FolderID].Hash;
                        else
                            updateHash.m_Hash = Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].Hash;

                        updateHash.m_IsFolder = Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].IsFolder;

                        if (!updateHash.m_IsFolder)
                            updateHash.m_Size = Server()->AutoUpdate()->m_aDir[i].m_aFiles[j].Size;
                        else
                            updateHash.m_Size = 0;

                        Server()->SendPackMsg(&updateHash, MSGFLAG_FLUSH, ClientID, false);
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < Server()->AutoUpdate()->m_aDir[0].m_aFiles.size(); i++)
            {
                CNetMsg_AutoUpdate_Hash updateHash;
                updateHash.m_Name = Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].Name;

                if (Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].IsFolder)
                    updateHash.m_Hash =Server()-> AutoUpdate()->m_aDir[Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].FolderID].Hash;
                else
                    updateHash.m_Hash = Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].Hash;

                updateHash.m_IsFolder = Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].IsFolder;
                updateHash.m_Size = Server()->AutoUpdate()->m_aDir[0].m_aFiles[i].Size;

                Server()->SendPackMsg(&updateHash, MSGFLAG_FLUSH, ClientID, false);
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}


bool CProtocol::ParseMsg(CUnpacker *Unpacker, int Msg, int ClientID)
{
    if (Server()->Game()->ClientState(ClientID) == CClientTdtw::STATE_EMPTY)
        return false;

    void *pRawMsg = m_NetHandler.SecureUnpackMsg(Msg, Unpacker);
    if (!pRawMsg)
    {
        Server()->Console()->PrintArg(IConsole::OUTPUT_LEVEL_DEBUG, "server",
                "dropped weird message '%s' (%d), failed on '%s'", m_NetHandler.GetMsgName(Msg), Msg, m_NetHandler.FailedMsgOn());

        return false;
    }

    return true;
}
