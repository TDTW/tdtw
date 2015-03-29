#include <engine/graphics.h>
#include "nui.h"
#include "ui/block.h"

CNui::CNui(class CGameClient *Client)
{
	m_pClient = Client;
}

CNuiElements *CNui::NewElement(CNuiElements::ELEMENT_TYPE Type, const char* Name)
{
	CNuiElements *Element;
	switch (Type)
	{
		case CNuiElements::BLOCK:
			Element = new CBlock(this, Name);
			break;
		/*case ELEMENT_TEXT:
			Element = new CElementText(m_pClient, this, Name);
			break;
		case ELEMENT_QUAD:
		case ELEMENT_TEE:*/
		default:
			Element = new CNuiElements(this, Name);
			break;
	}
	return Element;
}

CNuiElements *CNui::GetElement(CNuiElements::ELEMENT_TYPE Type, const char *Name)
{
	CNuiElements *pResultElement = SearchElement(Name);
	if (pResultElement == NULL)
		pResultElement = NewElement(Type, Name);

	pResultElement->m_pParent = ParseParent(Name);

	if(pResultElement->m_pParent != NULL)
		pResultElement->m_pParent->m_apChild.add(pResultElement);

	// TODO: ChangeLevel

	return pResultElement;
}

CNuiElements *CNui::SearchElement(char const *Name)
{
	for(int i = 0; i < m_aNuiElements.size(); i++)
	{
		if(!str_comp(m_aNuiElements[i]->m_pName,Name))
			return m_aNuiElements[i];
	}
	return NULL;
}

int CNui::SearchElementIndex(char const *Name)
{
	for(int i = 0; i < m_aNuiElements.size(); i++)
	{
		if(!str_comp(m_aNuiElements[i]->m_pName,Name))
			return i;
	}
	return -1;
}

CNuiElements *CNui::ParseParent(const char *Name)
{
	for(int i = str_length(Name) - 1; i >= 0; i--)
	{
		if(Name[i] == '.')
		{
			char *tempName = new char[i] (Name);
			return GetElement(CNuiElements::BLOCK, tempName);
		}
	}
	return NULL;
}

void CNui::DeleteElement(const char *Name) 
{
	int index = SearchElementIndex(Name);
	if(index != -1)
	{
		for (int i = 0; i < m_aNuiElements[index]->m_apChild.size(); ++i)
			m_aNuiElements.remove(m_aNuiElements[index]->m_apChild[i]);

		delete m_aNuiElements[index];
		m_aNuiElements.remove_index(index);
	}
}
