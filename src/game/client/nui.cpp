#include <engine/graphics.h>
#include <engine/client.h>
#include <engine/textrender.h>
#include <engine/shared/config.h>
#include "nui.h"

CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
	m_pUnderMouse = NULL;
	m_pActiveElement = NULL;
	m_pLastActiveElement = NULL;
	m_MousePosition = vec2(0,0);
}

void CControllerNui::OnMouseMove(vec2 MousePos)
{
	float Fac = (float)(g_Config.m_UiMousesens)/g_Config.m_InpMousesens;
	MousePos.x = MousePos.x*Fac;
	MousePos.y = MousePos.y*Fac;
	m_MousePosition.x += MousePos.x;
	m_MousePosition.y += MousePos.y;
	if(m_MousePosition.x < 0) m_MousePosition.x = 0;
	if(m_MousePosition.y < 0) m_MousePosition.y = 0;
	if(m_MousePosition.x > m_pClient->Graphics()->ScreenWidth()) m_MousePosition.x = m_pClient->Graphics()->ScreenWidth();
	if(m_MousePosition.y > m_pClient->Graphics()->ScreenHeight()) m_MousePosition.y = m_pClient->Graphics()->ScreenHeight();
}
vec2 CControllerNui::GetMousePos()
{
	CUIRect *pScreen = m_pClient->UI()->Screen();

	float MousePositionX = (m_MousePosition.x/(float)m_pClient->Graphics()->ScreenWidth())*pScreen->w;
	float MousePositionY = (m_MousePosition.y/(float)m_pClient->Graphics()->ScreenHeight())*pScreen->h;
	//dbg_msg("MOUSE", "%f %f", MousePositionX, MousePositionY);
	return vec2(MousePositionX, MousePositionY);
}


CNUIElements *CControllerNui::ParseElementName(const char *pSrc)
{
	for(int i = str_length(pSrc); i >= 0; i--)
	{
		if(pSrc[i] == '.')
		{
			char *name = new char[64];
			memset(name, 0, 64);
			str_copy(name, pSrc, i+1);
			return GetElement(ELEMENT_BLOCK, name);
		}
	}
	return 0;
}

CNUIElements *CControllerNui::GetElement(ELEMENT_TYPES Type, const char *Name)
{
	dbg_msg("GetElement", "%s", Name);
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (!strcmp(m_aNui[i]->m_pName, Name))
			return m_aNui[i];
	}
	CNUIElements *pNewElement = NULL;
	CNUIElements *pParent = ParseElementName(Name);
	switch (Type)
	{
		case ELEMENT_BLOCK:
			pNewElement = new CElementBlock(m_pClient, this, Name);
			break;
		case ELEMENT_TEXT:
			pNewElement = new CElementText(m_pClient, this, Name);
			break;
		case ELEMENT_QUAD:
		case ELEMENT_TEE:
		default:
			//NewElement = new CNUIElements(m_pClient, this, Name);
			break;
	}
	if(pParent != NULL && pNewElement != NULL)
		pNewElement->m_pParent = pParent;
	if(pParent)
		dbg_msg("GetElement", "Created %s->%s",pParent->m_pName, pNewElement->m_pName);
	else dbg_msg("GetElement", "Created %s", pNewElement->m_pName);
	m_aNui.add(pNewElement);
	return pNewElement;
}

void CControllerNui::RemoveElement(CNUIElements *pNui)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (m_aNui[i] == pNui)
		{
			m_aNui.remove_index(i);
			break;
		}
	}
}

class IClient *CNUIElements::Client() const
{
	return m_pClient->Client();
}

class IGraphics *CNUIElements::Graphics() const
{
	return m_pClient->Graphics();
}

class ITextRender *CNUIElements::TextRender() const
{
	return m_pClient->TextRender();
}

class CRenderTools *CNUIElements::RenderTools() const
{
	return m_pClient->RenderTools();
}
