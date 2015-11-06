#include <engine/graphics.h>
#include "ui/text1.h"
#include "nui.h"
#include "ui/block1.h"

CNui::CNui(class CGameClient *Client)
{
	m_pClient = Client;
}

CNuiElements *CNui::NewElement(CNuiElements::ELEMENT_TYPE Type, const char *Name)
{
	CNuiElements *Element;

	switch (Type)
	{
		case CNuiElements::BLOCK:
			Element = new CBlock(this, Name);
			break;
		case CNuiElements::TEXT:
			Element = new CText(this, Name);
			break;
		/*case ELEMENT_QUAD:
		case ELEMENT_TEE:*/
		default:
			Element = new CBlock(this, Name);
			break;
	}
	dbg_msg("Nui", "Element %s created x:%.2f y:%.2f", Name, Element->GetChildPosGlobal().x, Element->GetChildPosGlobal().y);
	m_aNuiElements.add(Element);

	Element->m_pParent = ParseParent(Name);

	if(Element->m_pParent != NULL)
		Element->m_pParent->m_apChild.add(Element);

	return Element;
}

CNuiElements *CNui::GetElement(CNuiElements::ELEMENT_TYPE Type, const char *Name)
{
	CNuiElements *pResultElement = SearchElement(Name);
	if (pResultElement == NULL)
		pResultElement = NewElement(Type, Name);

	// TODO: ChangeLevel

	return pResultElement;
}

CNuiElements *CNui::SearchElement(char const *Name)
{
	for(int i = 0; i < m_aNuiElements.size(); i++)
	{
		if(!str_comp(m_aNuiElements[i]->m_pName, Name))
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
			char tempName[i+1];
			str_copy(tempName, Name, sizeof(tempName));
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
		{
			DeleteElement(m_aNuiElements[index]->m_apChild[i]->m_pName);
			m_aNuiElements.remove(m_aNuiElements[index]->m_apChild[i]);
		}

		delete m_aNuiElements[index];
		m_aNuiElements.remove_index(index);
	}
}
