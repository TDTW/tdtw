#include <engine/graphics.h>
#include <engine/client.h>
#include <engine/textrender.h>
#include "nui.h"

CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
}

CNUIElements *CControllerNui::GetElement(ELEMENT_TYPES Type, const char *Name)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (!strcmp(m_aNui[i]->m_pName, Name))
			return m_aNui[i];
	}
	CNUIElements *NewElement = NULL;
	switch (Type)
	{
		case ELEMENT_BLOCK:
			NewElement = new CElementBlock(m_pClient, this, Name);
			break;
		case ELEMENT_TEXT:
			NewElement = new CElementText(m_pClient, this, Name);
			break;
		case ELEMENT_QUAD:
		case ELEMENT_TEE:
		default:
			//NewElement = new CNUIElements(m_pClient, this, Name);
			break;
	}
	m_aNui.add(NewElement);
	return NewElement;
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