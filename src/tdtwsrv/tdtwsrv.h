#ifndef TDTWSRV_TDTWSRV_H
#define TDTWSRV_TDTWSRV_H
#include <engine/kernel.h>
#include <engine/message.h>

class ITDTWSrv : public IInterface
{
	MACRO_INTERFACE("tdtwsrv", 0)
public:

	virtual int SendMsg(CMsgPacker *pMsg, int Flags, int ClientID) = 0;
	virtual int SendMsgEx(CMsgPacker *pMsg, int Flags, int ClientID, bool System) = 0;
	virtual void Run() = 0;
	virtual void RequestInterfaces() = 0;

	template<class T>
	int SendPackMsg(T *pMsg, int Flags, int ClientID, bool System = false)
	{
		CMsgPacker Packer(pMsg->MsgID());
		if (pMsg->Pack(&Packer))
			return -1;
		return SendMsgEx(&Packer, Flags, ClientID, System);
	}

    class IStorage *Storage() { return m_pStorage; }
    class IConsole *Console() { return m_pConsole; }
    class IAutoUpdate *AutoUpdate() { return m_pAutoUpdate; }
    class IGame *Game() { return m_pGame; }

protected:
    class IStorage *m_pStorage;
    class IConsole *m_pConsole;
    class IAutoUpdate *m_pAutoUpdate;
    class IGame *m_pGame;
};

extern ITDTWSrv *CreateTDTWServer();
#endif