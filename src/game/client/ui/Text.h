#include "elements.h"

#ifndef GAME_CLIENT_UI_TEXT_H
#define GAME_CLIENT_UI_TEXT_H

class CText : public CNuiElements
{
public:
	enum TEXT_ALIGN
	{
		ALIGN_CENTER,
		ALIGN_LEFT,
		ALIGN_RIGHT
	};
	CText(class CNui *pNui, const char *Name);

	virtual void SetText(bool TextUpdate, CText::TEXT_ALIGN Align, const char *pText, ...);
};

#endif //GAME_CLIENT_UI_TEXT_H
