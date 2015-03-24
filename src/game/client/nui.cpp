#include <engine/graphics.h>
#include "nui.h"

CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
	m_pUnderMouse = NULL;
	m_pActiveElement = NULL;
	m_pLastActiveElement = NULL;
	m_pMouse = new CNuiMouse(this);
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

void CControllerNui::ChangeElementLevel()
{
	for(int i=0; i < m_aNui.size(); i++)
		for(int j=i; j < m_aNui.size(); j++)
			if(m_aNui[j]->GetRenderLevel() < m_aNui[i]->GetRenderLevel())
				m_aNui.swap(i, j);
}

CNUIElements *CControllerNui::GetElementByName(char const *Name)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (!strcmp(m_aNui[i]->m_pName, Name))
			return  m_aNui[i];
	}
	return NULL;
}

CNUIElements *CControllerNui::GetElement(ELEMENT_TYPES Type, const char Name[])
{
	CNUIElements *pResult = GetElementByName(Name);
	if(pResult != NULL)
		return pResult;

	CNUIElements *pNewElement = NULL;
	CNUIElements *pParent = ParseElementName(Name);

	while(pNewElement == NULL)
	{
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
	}

	if(pParent != NULL)
		pNewElement->m_pParent = pParent;

	m_aNui.add(pNewElement);
	ChangeElementLevel(); //TODO: rewrite

	return pNewElement;
}

void CControllerNui::RemoveElement(char const *pName)
{
	CNUIElements *pRemovedElement = GetElementByName(pName);
	if(pRemovedElement != NULL)
	{
		for(int j=0; j < m_aNui.size(); j++)
		{
			if(m_aNui[j]->m_pParent == pRemovedElement)
				m_aNui.remove_index_fast(j);
		}
		m_aNui.remove(pRemovedElement);
		ChangeElementLevel();
	}
}
