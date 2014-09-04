#ifndef TDTWSRV_AUTOUPDATE_H
#define TDTWSRV_AUTOUPDATE_H

#include <base\system.h>
#include <base\tl\array.h>

#include <engine\shared\datafile.h>
#include <engine\kernel.h>
#include <engine\storage.h>
#include <engine\console.h>
#include <engine\autoupdate.h>

class CAutoUpdate : public IAutoUpdate
{	
public:
	CAutoUpdate();

	virtual void RequestInterfaces();
	virtual void CheckHash();

	class IConsole *Console() { return m_pConsole; }
	class IStorage *Storage() { return m_pStorage; }
private:
	class IConsole *m_pConsole;
	class IStorage *m_pStorage;
};

#endif