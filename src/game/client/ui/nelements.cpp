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

	m_PosLocal = CValue(this);
	m_PosGlobal = vec4(0, 0, 0, 0);

	m_Color = CValue(this);
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

	m_pVisualArg = NULL;
	m_pEventArg = NULL;


	m_EndLifeTimeCallback = NULL;
	m_BeforeDieCallback = NULL;
	m_pEndLifeTimeArg = NULL;
	m_EndLifeTimeCB = false;
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
	m_pVisualArg = Arg;
}

void CNUIElements::SetCallbacksEvents(CallBack Click, CallBack DblClick, CallBack RightClick, void *Arg)
{
	m_Click = Click;
	m_DblClick = DblClick;
	m_RightClick = RightClick;
	m_UseEventMouse = true;
	m_UseVisualMouse = true;
	m_pEventArg = Arg;
}

void CNUIElements::SetEndLifeTimeCallback(CallBack EndTime, CallBack BeforeDie, void *Arg)
{
	m_EndLifeTimeCallback = EndTime;
	m_BeforeDieCallback = BeforeDie;
	m_pEndLifeTimeArg = Arg;
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
			m_FocusOn(this, m_pVisualArg);
	}
	else if(m_pControllerNui->m_pUnderMouse == this && !MouseInside()) //FOCUS OUT
	{
		m_pControllerNui->m_pUnderMouse = NULL;
		if(m_FocusOut)
			m_FocusOut(this, m_pVisualArg);
		m_pControllerNui->m_pActiveElement = NULL;
	}

	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyDown(KEY_MOUSE_1))
	{
		if(m_MouseDown)
			m_MouseDown(this, m_pVisualArg);
		m_pControllerNui->m_pActiveElement = this;
	}
	else if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_1))
	{
		if(m_MouseUp)
			m_MouseUp(this, m_pVisualArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
	}
}

void CNUIElements::CheckMouseEvent()
{
	if (m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_1))
	{
		if (m_Click)
			m_Click(this, m_pEventArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
	}
	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->MouseDoubleClick())
	{
		if(m_DblClick && !m_Click)
			m_DblClick(this, m_pEventArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
	}
	if(m_pControllerNui->m_pUnderMouse == this && m_pClient->Input()->KeyUp(KEY_MOUSE_2))
	{
		if(m_RightClick && !m_DblClick)
			m_RightClick(this, m_pEventArg);
		m_pControllerNui->m_pLastActiveElement = m_pControllerNui->m_pActiveElement;
	}
}

void CNUIElements::PreRender()
{
	if(m_EndLife && m_EndLifeTime < time_get() && m_EndLifeTimeCallback && !m_EndLifeTimeCB)
	{
		m_EndLifeTimeCallback(this, m_pEndLifeTimeArg);
		m_EndLifeTimeCB = true;
	}

	if(m_EndLife && m_EndLifeTime < time_get() && !m_DieProcess)
	{
		m_Color.Init(vec4(m_Color.m_Value.r, m_Color.m_Value.g, m_Color.m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_PosLocal.InitPlus(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}

	if(m_PosLocal.m_AnimTime <= time_get() && time_get() <= m_PosLocal.m_AnimEndTime)
		m_PosLocal.Recalculate();
	else if(!m_PosLocal.m_AnimEnded)
		m_PosLocal.EndAnimation();

	if(m_Color.m_AnimTime <= time_get() && time_get() <= m_Color.m_AnimEndTime)
		m_Color.Recalculate();
	else if(!m_Color.m_AnimEnded)
		m_Color.EndAnimation();

	if (m_pParent != NULL)
		m_PosGlobal = m_pParent->GetChildPosGlobal();
	else
		m_PosGlobal = vec4(0, 0, 0, 0);

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
	{
		m_pControllerNui->RemoveElement(m_pName);
		if(m_BeforeDieCallback)
			m_BeforeDieCallback(this, m_pEndLifeTimeArg);
	}
	if (m_pParent && m_pParent->GetClipEnable() && !m_StopClipByParent)
		Graphics()->ClipDisable();
	
	if(m_UseEventMouse && !m_DieProcess)
		CheckMouseEvent();
}

vec4 CNUIElements::GetChildPosGlobal()
{
	return m_PosGlobal + m_PosLocal.m_Value;
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
	Pos.w = m_PosLocal.m_Value.w;
	Pos.h = m_PosLocal.m_Value.h;

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
	m_Renderlevel = Level;
	m_pControllerNui->ChangeElementLevel();
}
