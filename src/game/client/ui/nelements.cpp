#include <engine/client.h>
#include <base/vmath.h>
#include <engine/input.h>
#include "nuitext.h"
#include <engine/graphics.h>
#include <game/client/nui.h>
#include <engine/keys.h>
#include <engine/external/zlib/deflate.h>

CNUIElements::CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
{
	m_pName = Name;
	m_pParent = 0;
	m_pClient = pClient;
	m_pControllerNui = pControllerNui;

	m_pPosLocal = CValue(this);
	m_pPosGlobal = vec4(0, 0, 0, 0);

	m_pColor = CValue(this);
	m_Renderlevel = NORMAL;

	m_DieProcess = false;
	m_EndLife = false;
	m_EndLifeDur = 0.0f;
	m_EndLifeTime = 0.0f;

	m_DieCoord = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_DieAnimation = Default;

	m_ClipUsed = false;

	m_FocusOn = NULL;
	m_FocusOut = NULL;
	m_MouseDown = NULL;
	m_MouseUp = NULL;

	m_Click = NULL;
	m_DblClick = NULL;
	m_RightClick = NULL;

	m_UseVisualMouse = false;
	m_UseEventMouse = false;

	m_VisualArg = NULL;
	m_EventArg = NULL;
}

void CNUIElements::SetLifeTime(int LifeTime, float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = time_get() + LifeTime * time_freq();
}

void CNUIElements::SetCallbacksVisual(CallBack FocusOn, CallBack FocusOut, CallBack MouseDown, CallBack MouseUp, void *Arg)
{
	m_FocusOn = FocusOn;
	m_FocusOut = FocusOut;
	m_MouseDown = MouseDown;
	m_MouseUp = MouseUp;
	m_UseVisualMouse = true;
	m_VisualArg = Arg;
}

void CNUIElements::SetCallbacksEvents(CallBack Click, CallBack DblClick, CallBack RightClick, void *Arg)
{
	m_Click = Click;
	m_DblClick = DblClick;
	m_RightClick = RightClick;
	m_UseEventMouse = true;
	m_UseVisualMouse = true;
	m_EventArg = Arg;
}

void CNUIElements::SetEndLife(float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = time_get();
}

void CNUIElements::SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord)
{
	m_DieAnimation = animation_type;
	m_DieCoord = Coord;
}

bool CNUIElements::MouseInside()
{
		return m_pControllerNui->GetMousePos().x >= GetClipWithoutScale().x &&
				m_pControllerNui->GetMousePos().x <= GetClipWithoutScale().x+GetClipWithoutScale().w &&
				m_pControllerNui->GetMousePos().y >= GetClipWithoutScale().y &&
				m_pControllerNui->GetMousePos().y <= GetClipWithoutScale().y+GetClipWithoutScale().h;
}

void CNUIElements::CheckMouseVisual()
{
	if(MouseInside() && m_pControllerNui->m_pUnderMouse != this) //FOCUS ON
	{
		m_pControllerNui->m_pUnderMouse = this;
		if(m_FocusOn)
			m_FocusOn(this, m_VisualArg);
		//dbg_msg("ELEMENT", "FOCUS ON %s", m_pName);
	}
	else if(m_pControllerNui->m_pUnderMouse == this && !MouseInside()) //FOCUS OUT
	{
		m_pControllerNui->m_pUnderMouse = NULL;
		if(m_FocusOut)
			m_FocusOut(this, m_VisualArg);
		m_pControllerNui->m_pActiveElement = NULL;
		//dbg_msg("ELEMENT", "FOCUS OUT %s", m_pName);
	}

	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyDown(KEY_MOUSE_1))
	{
		if(m_MouseDown)
			m_MouseDown(this, m_VisualArg);
		m_pControllerNui->m_pActiveElement = this;
		//dbg_msg("ELEMENT", "MOUSE DOWN %s", m_pName);
	}
	else if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_1))
	{
		if(m_MouseUp)
			m_MouseUp(this, m_VisualArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
		//dbg_msg("ELEMENT", "MOUSE UP %s", m_pName);
	}
}

void CNUIElements::CheckMouseEvent()
{
	if (m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_1))
	{
		if (m_Click)
			m_Click(this, m_EventArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
		//dbg_msg("ELEMENT", "MOUSE UP %s", m_pName);
	}
	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->MouseDoubleClick())
	{
		if(m_DblClick && !m_Click)
			m_DblClick(this, m_EventArg);
		//dbg_msg("ELEMENT", "Dbl Click %s",m_pName);
	}
	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_2))
	{
		if(m_RightClick && !m_DblClick)
			m_RightClick(this, m_EventArg);
		//dbg_msg("ELEMENT", "Right Click %s",m_pName);
	}
}

void CNUIElements::PreRender()
{
	if(m_EndLife && m_EndLifeTime < time_get() && !m_DieProcess)
	{
		m_pColor.Init(vec4(m_pColor.m_Value.r, m_pColor.m_Value.g, m_pColor.m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_pPosLocal.Init(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}

	if(m_pPosLocal.m_AnimTime <= time_get() && time_get() <= m_pPosLocal.m_AnimEndTime)
		m_pPosLocal.Recalculate();
	else if(!m_pPosLocal.m_AnimEnded)
		m_pPosLocal.EndAnimation();

	if(m_pColor.m_AnimTime <= time_get() && time_get() <= m_pColor.m_AnimEndTime)
		m_pColor.Recalculate();
	else if(!m_pColor.m_AnimEnded)
		m_pColor.EndAnimation();

	if (m_pParent != NULL)
		m_pPosGlobal = m_pParent->GetChildPosGlobal();
	else
		m_pPosGlobal = vec4(0, 0, 0, 0);

	if (m_pParent && m_pParent->GetClipEnable() && !m_StopClipByParent)
	{
		vec4 Pos = m_pParent->GetClipPos();
		Graphics()->ClipEnable((int) Pos.x, (int) Pos.y, (int) Pos.w, (int) Pos.h);
		//dbg_msg("olol", "%d %d %d %d", (int)Pos.x, (int)Pos.y, (int)Pos.w, (int)Pos.h);
	}

	if(m_UseVisualMouse && !m_DieProcess)
		CheckMouseVisual();
}

void CNUIElements::PostRender()
{
	if(m_DieProcess && (m_EndLifeTime + m_EndLifeDur * time_freq() < time_get()))
		m_pControllerNui->RemoveElement(m_pName);

	if (m_pParent && m_pParent->GetClipEnable() && !m_StopClipByParent)
		Graphics()->ClipDisable();
	
	if(m_UseEventMouse && !m_DieProcess)
		CheckMouseEvent();
}

vec4 CNUIElements::GetChildPosGlobal()
{
	return m_pPosGlobal + m_pPosLocal.m_Value;
}

void CNUIElements::SetChildClipping()
{
	m_ClipUsed = true;

	float Aspect = Graphics()->ScreenAspect();
	float h = 600;
	float w = Aspect * h;

	m_XScale = Graphics()->ScreenWidth() / w;
	m_YScale = Graphics()->ScreenHeight() / h;
	dbg_msg("olol", "%f %f", m_XScale, m_YScale);
}

vec4 CNUIElements::GetClipPos()
{
	vec4 Pos = GetClipWithoutScale();

	Pos.x *= m_XScale;
	Pos.y *= m_YScale;
	Pos.w *= m_XScale;
	Pos.h *= m_YScale;
	return Pos;
}

vec4 CNUIElements::GetClipWithoutScale()
{
	vec4 Pos;
	Pos.x = GetChildPosGlobal().x;
	Pos.y = GetChildPosGlobal().y;
	Pos.w = m_pPosLocal.m_Value.w;
	Pos.h = m_pPosLocal.m_Value.h;

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

void CNUIElements::SetRenderLevel(RENDER_LEVEL Level)
{
	m_pControllerNui->ChangeElementLevel(this, Level);
}