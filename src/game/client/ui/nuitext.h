#ifndef GAME_CLIENT_UI_NUITEXT_H
#define GAME_CLIENT_UI_NUITEXT_H

#include <base/tl/array.h>
#include "nelements.h"
#include <stdarg.h>
#include <stdio.h>

class CElementText : public CNUIElements
{
public:
	CElementText(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name);

	virtual class CValue *GetColorOutline()	{return &m_pColorOutline;};

	virtual void Render();
	virtual void SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...);

	virtual void PreRender();

private:
	void ParseTypes(const char *String, va_list pArguments);

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

#endif //GAME_CLIENT_UI_NUITEXT_H
