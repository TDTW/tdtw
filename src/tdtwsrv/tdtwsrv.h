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
	virtual const char *LatestVersion() = 0;
};

extern ITDTWSrv *CreateTDTWServer();
#endif