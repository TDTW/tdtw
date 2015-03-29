/*
#include <engine/shared/config.h>
#include <engine/graphics.h>
#include "nuimouse.h"

CNuiMouse::CNuiMouse(CControllerNui *pNUI)
{
	m_MousePosition = vec2(0, 0);
	m_pNUI = pNUI;
}

void CNuiMouse::OnMove(vec2 MousePos)
{
	float Fac = (float)(g_Config.m_UiMousesens)/g_Config.m_InpMousesens;
	MousePos.x = MousePos.x*Fac;
	MousePos.y = MousePos.y*Fac;
	m_MousePosition.x += MousePos.x;
	m_MousePosition.y += MousePos.y;
	if(m_MousePosition.x < 0) m_MousePosition.x = 0;
	if(m_MousePosition.y < 0) m_MousePosition.y = 0;
	if(m_MousePosition.x > Client()->Graphics()->ScreenWidth()) m_MousePosition.x = Client()->Graphics()->ScreenWidth();
	if(m_MousePosition.y > Client()->Graphics()->ScreenHeight()) m_MousePosition.y = Client()->Graphics()->ScreenHeight();
}

vec2 CNuiMouse::GetPos()
{
	CUIRect *pScreen = Client()->UI()->Screen();

	float MousePositionX = (m_MousePosition.x/(float)Client()->Graphics()->ScreenWidth())*pScreen->w;
	float MousePositionY = (m_MousePosition.y/(float)Client()->Graphics()->ScreenHeight())*pScreen->h;

	return vec2(MousePositionX, MousePositionY);
}

vec2 CNuiMouse::GetPosClamp(vec4 ClampBox)
{
	m_MousePosition.x = clamp(m_MousePosition.x, ClampBox.x, ClampBox.w);
	m_MousePosition.y = clamp(m_MousePosition.y, ClampBox.y, ClampBox.h);
	return m_MousePosition;
}

class CGameClient *CNuiMouse::Client() const {return m_pNUI->Client();}
*/
