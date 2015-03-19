#include <engine/graphics.h>
#include <engine/client.h>
#include <engine/textrender.h>
#include "nui.h"

CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
}

CNUIElements *CControllerNui::ParseElementName(const char *pSrc)
{
	CNUIElements *pParent = 0;
	bool First = true;
	for(int i = 0; i < str_length(pSrc) ; i++)
	{
		if(pSrc[i] == '.')
		{
			char name[64] = {0};
			mem_copy(name, pSrc, i);
			pParent = GetElement(ELEMENT_BLOCK, name);

		}
	}
	return pParent == 0 ? NULL : pParent;
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
	dbg_msg("GetElement", "Created");
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