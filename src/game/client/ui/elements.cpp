#include <engine/graphics.h>
#include <game/client/nui.h>
#include "elements.h"

CNuiElements::CNuiElements(class CNui *pNui, const char *Name)
{
    m_pName = new char[str_length(Name)+1];
	m_pPosLocal = new CValue();
	m_pColor = new CValue();

	str_copy(m_pName, Name, str_length(Name)+1);

    m_pNui = pNui;
    m_pParent = NULL;
	m_apChild.clear();
	m_PosGlobal = vec4(0, 0, 0, 0);
}

CNuiElements::~CNuiElements()
{
	delete m_pName;
	delete m_pPosLocal;
	delete m_pColor;
}

void CNuiElements::PreRender()
{
	m_pPosLocal->PreRender();
	m_pColor->PreRender();

	if (m_pParent != NULL)
		m_PosGlobal = m_pParent->GetChildPosGlobal();
	else
		m_PosGlobal = vec4(0, 0, 0, 0);

	//callback end animation
	//if(m_AnimEnd)
	//	m_AnimEnd(this, m_pAnimArg);

	if (m_pParent && m_pParent->GetClipEnable() && !m_StopClipByParent)
	{
		vec4 Pos = m_pParent->GetClipPos();
		Graphics()->ClipEnable((int) Pos.x, (int) Pos.y, (int) Pos.w, (int) Pos.h);
	}
}

void CNuiElements::PostRender()
{

}

void CNuiElements::SetChildClipping()
{
	m_ClipUsed = true;

	float Aspect = Graphics()->ScreenAspect();
	float h = 600;
	float w = Aspect * h;

	m_XScale = Graphics()->ScreenWidth() / w;
	m_YScale = Graphics()->ScreenHeight() / h;
}

vec4 CNuiElements::GetClipPos()
{
	vec4 Pos = GetClipWithoutScale();

	Pos.x *= m_XScale;
	Pos.y *= m_YScale;
	Pos.w *= m_XScale;
	Pos.h *= m_YScale;
	return Pos;
}

vec4 CNuiElements::GetClipWithoutScale()
{
	vec4 Pos;
	Pos.x = GetChildPosGlobal().x;
	Pos.y = GetChildPosGlobal().y;
	Pos.w = m_pPosLocal->GetValue().w;
	Pos.h = m_pPosLocal->GetValue().h;

	if (m_pParent && m_pParent->GetClipEnable())
	{
		vec4 ParentPos = m_pParent->GetClipWithoutScale();
		if (Pos.x < ParentPos.x)
			Pos.x = ParentPos.x;
		if (Pos.y < ParentPos.y)
			Pos.y = ParentPos.y;
		if (Pos.x + Pos.w > ParentPos.x + ParentPos.w)
			Pos.w = (ParentPos.x + ParentPos.w) - Pos.x;
		if (Pos.y + Pos.h > ParentPos.y + ParentPos.h)
			Pos.h = (ParentPos.y + ParentPos.h) - Pos.y;
	}
	return Pos;
}

class IClient *CNuiElements::Client() const{return m_pNui->Client()->Client();}
class IGraphics *CNuiElements::Graphics() const{return m_pNui->Client()->Graphics();}
class ITextRender *CNuiElements::TextRender() const{return m_pNui->Client()->TextRender();}
class CRenderTools *CNuiElements::RenderTools() const{return m_pNui->Client()->RenderTools();}