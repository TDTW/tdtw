#ifndef GAME_CLIENT_UI_TEXT_H
#define GAME_CLIENT_UI_TEXT_H

#include <stdarg.h>
#include "elements.h"

class CText : public CNuiElements
{
public:
	CText(class CNui *pNui, const char *Name);

	virtual void Render();
	virtual void SetText(bool TextUpdate, CNuiElements::TEXT_ALIGN Align, const char *pText, ...);
private:
	void ParseTypes(const char *Template, va_list Arguments);

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

	TEXT_ALIGN m_Align;
	bool m_TextUpdate;
	char *m_pTextTemplate;
	char *m_pRenderText;

	array<sArg *> m_apArgs;

};

#endif //GAME_CLIENT_UI_TEXT_H
