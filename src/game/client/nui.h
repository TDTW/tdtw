#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <base/tl/array.h>
#include <game/client/ui/nelements.h>
#include <game/client/ui/value.h>
#include <game/client/ui/nuiblock.h>
#include <game/client/ui/nuitext.h>
#include <game/client/ui/nuimouse.h>
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

	CNUIElements *GetElement(ELEMENT_TYPES Type, const char Name[]);
	CNUIElements *GetElement(int num){return m_aNui[num];}
	CNUIElements *ParseElementName(const char *pSrc);
	void RemoveElement(char const *pName);
	void ChangeElementLevel();

	CNUIElements *m_pUnderMouse;
	CNUIElements *m_pActiveElement;
	CNUIElements *m_pLastActiveElement;

	int GetSize() {return m_aNui.size();}

	class CGameClient *Client() { return m_pClient; }
	class CNuiMouse *Mouse() { return m_pMouse; }
private:
	class CGameClient *m_pClient;
	array<CNUIElements *> m_aNui;
	class CNuiMouse *m_pMouse;
	CNUIElements *GetElementByName(char const *Name);
};

#endif //GAME_CLIENT_NUI_H
