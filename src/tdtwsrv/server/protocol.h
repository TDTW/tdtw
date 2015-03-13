#ifndef TDTWSRV_TDTWSRV_PROTOCOL_H
#define TDTWSRV_TDTWSRV_PROTOCOL_H

#include <engine/shared/packer.h>
#include <engine/console.h>
#include <engine/storage.h>
#include <engine/config.h>
#include <engine/autoupdate.h>

#include <game/generated/protocol_tdtw.h>

#include <tdtwsrv/game.h>
#include <tdtwsrv/tdtwsrv.h>

class CProtocol
{
public:
    CProtocol(ITDTWSrv *pServer);
    ~CProtocol();

    bool Protocol(CUnpacker *Unpacker, int Msg, int Sys, int ClientID);
    bool ParseSystemMsg(CUnpacker *Unpacker, int Msg, int ClientID);
    bool ParseMsg(CUnpacker *Unpacker, int Msg, int ClientID);


    ITDTWSrv *Server() { return m_pServer; }
private:
    ITDTWSrv *m_pServer;
    CNetObjHandlerTdtw m_NetHandler;
};

#endif // TDTWSRV_TDTWSRV_PROTOCOL_H