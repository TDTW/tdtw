/*
#ifndef GAME_CLIENT_UI_NUIMOUSE_H
#define GAME_CLIENT_UI_NUIMOUSE_H

#include <base/vmath.h>
#include <game/client/nui.h>

class CNuiMouse
{
public:
	CNuiMouse(class CControllerNui *pNUI);
	void OnMove(vec2 MousePos);
	vec2 GetPos();
	vec2 GetPosClamp(vec4 ClampBox);

	class CGameClient *Client() const;
private:
	class CControllerNui *m_pNUI;

	vec2 m_MousePosition;
};

#endif //GAME_CLIENT_UI_NUIMOUSE_H
*/
