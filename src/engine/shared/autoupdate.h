#ifndef TDTWSRV_AUTOUPDATE_H
#define TDTWSRV_AUTOUPDATE_H

#include <base/system.h>
#include <base/tl/array.h>

#include <engine/shared/datafile.h>
#include <engine/kernel.h>
#include <engine/storage.h>
#include <engine/console.h>
#include <engine/autoupdate.h>
#include <game/version.h>

class CAutoUpdate : public IAutoUpdate
{	
public:
	CAutoUpdate();

	virtual void RequestInterfaces();
	virtual void CheckHash();
	virtual bool CheckVersion(char *Version);
	virtual void ReplaceFileUpdate(char *FileName);
	virtual void SetNeedReplace(bool Replace) { NeedReplace = Replace; }
	virtual bool GetNeedReplace() { return NeedReplace; }
    virtual void SetBinaryTempNumber(int Num) { SecretNum = Num; }
	class IConsole *Console() { return m_pConsole; }
	class IStorage *Storage() { return m_pStorage; }
private:
	bool NeedReplace;
	class IConsole *m_pConsole;
	class IStorage *m_pStorage;
    int SecretNum;
};

#endif