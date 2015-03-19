#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <base/tl/array.h>
#include <game/client/ui/nelements.h>
#include <game/client/ui/value.h>
#include <game/client/ui/nuiblock.h>
#include <game/client/ui/nuitext.h>
#include "render.h"
#include "gameclient.h"

enum ELEMENT_TYPES
{
	ELEMENT_BLOCK,
	ELEMENT_TEXT,
	ELEMENT_QUAD,
	ELEMENT_TEE
};

class CControllerNui
{
public:
	CControllerNui(class CGameClient *Client);

	CNUIElements *GetElement(ELEMENT_TYPES Type, const char *Name);
	CNUIElements *ParseElementName(const char *pSrc);
	int GetSize() {return m_aNui.size();}

	CNUIElements *GetElement(int num)
	{
		return m_aNui[num];
	}
	void RemoveElement(CNUIElements *pNui);
private:
	class CGameClient *m_pClient;

	array<CNUIElements *> m_aNui;
};

#endif //GAME_CLIENT_NUI_H
