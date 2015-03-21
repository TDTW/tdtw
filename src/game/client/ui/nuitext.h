#ifndef _TDTW_NUITEXT_H_
#define _TDTW_NUITEXT_H_


#include <base/tl/array.h>
#include "nelements.h"


class CElementText : public CNUIElements
{
public:
	CElementText(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name);

	virtual class CValue *GetColorOutline()
	{
		return &m_pColorOutline;
	};

	virtual void Render();
	virtual void SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...);

	virtual void PreRender();

private:
	void ParseTypes(const char *String);

	enum TYPES
	{
		INT,
		FLOAT,
		STRING,
		LONG
	};

	struct sArg
	{
		void *m_Args;
		TYPES m_ArgType;
		int m_EndPos;
	};

	CValue m_pColorOutline;
	TEXT_ALIGN m_Align;

	bool m_TextUpdate;
	char m_UnUpdatedText[255];    // TODO: Unlimited Text Size
	const char *m_pTextTemplate;

	array<sArg *> m_aArgs;
};

#endif //_TDTW_NUITEXT_H_
