#include <game/client/nui.h>
#include "elements.h"

CNuiElements::CNuiElements(class CNui *pNui, const char *Name)
{
    m_pName = new char[str_length(Name)+1];
	str_copy(m_pName, Name, str_length(Name)+1);
    m_pNui = pNui;
    m_pParent = NULL;
	m_apChild.clear();
	m_pPosLocal = new CValue();
	m_PosGlobal = vec4(0, 0, 0, 0);
	m_pColor = new CValue();
}

CNuiElements::~CNuiElements()
{
	delete m_pName;
	delete m_pPosLocal;
	delete m_pColor;
}

void CNuiElements::PreRender()
{
	if (m_pParent != NULL)
		m_PosGlobal = m_pParent->GetChildPosGlobal();
	else
		m_PosGlobal = vec4(0, 0, 0, 0);
}

void CNuiElements::PostRender()
{

}

class IClient *CNuiElements::Client() const{return m_pNui->Client()->Client();}
class IGraphics *CNuiElements::Graphics() const{return m_pNui->Client()->Graphics();}
class ITextRender *CNuiElements::TextRender() const{return m_pNui->Client()->TextRender();}
class CRenderTools *CNuiElements::RenderTools() const{return m_pNui->Client()->RenderTools();}