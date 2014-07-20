#ifndef ENGINE_TDTWSERVER_H
#define ENGINE_TDTWSERVER_H
#include "kernel.h"
#include "message.h"

class ITDTWServer : public IInterface
{
	MACRO_INTERFACE("tdtwserver", 0);
public:
	virtual void Recv(struct CNetChunk *Chunk) = 0;
	virtual void Init() = 0;
};
extern ITDTWServer *CreateTDTWServer();
#endif