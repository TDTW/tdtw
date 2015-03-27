#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <base/tl/array.h>
#include <game/client/ui/elements.h>
/*#include <game/client/ui/value.h>
#include <game/client/ui/nuiblock.h>
#include <game/client/ui/nuitext.h>
#include <game/client/ui/nuimouse.h>
#include <game/client/ui2/nelements.h>*/
#include "render.h"
#include "gameclient.h"


class CNui
{
public:
	CNui(class CGameClient *Client);
	CNuiElements *NewElement(CNuiElements::ELEMENT_TYPE Type, const char *Name);
	CNuiElements *GetElement(CNuiElements::ELEMENT_TYPE Type, const char *Name);
	void DeleteElement(const char *Name);

	int GetElementNumber() { m_aNuiElements.size(); }
	CNuiElements *GetElement(int Index) { return m_aNuiElements[Index]; }
private:
	array <CNuiElements*> m_aNuiElements;

	CGameClient *m_pClient;

	CNuiElements *ParseParent(const char *Name);
	CNuiElements *SearchElement(char const *Name);
	int SearchElementIndex(char const *Name);
};

/*
class CControllerNui
{
public:
	void ChangeElementLevel();

	CNUIElements *m_pUnderMouse;
	CNUIElements *m_pActiveElement;
	CNUIElements *m_pLastActiveElement;

	int GetSize() {return m_aNui.size();}

	class CNuiMouse *Mouse() { return m_pMouse; }
private:
	class CNuiMouse *m_pMouse;
};*/

#endif //GAME_CLIENT_NUI_H
