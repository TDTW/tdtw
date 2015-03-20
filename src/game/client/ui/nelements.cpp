#include <engine/client.h>
#include <base/vmath.h>
#include "nuitext.h"
#include "value.h"
#include <engine/graphics.h>
#include <game/client/nui.h>
#include <engine/external/zlib/deflate.h>

CNUIElements::CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
{
	m_pName = Name;
	m_pParent = 0;
	m_pClient = pClient;
	m_pControllerNui = pControllerNui;

	m_pPosLocal = new CValue(this);
	m_pPosGlobal = vec4(0, 0, 0, 0);
	m_pColor = new CValue(this);
	m_Renderlevel = NORMAL;

	m_DieProcess = false;
	m_EndLife = false;
	m_EndLifeDur = 0.0f;
	m_EndLifeTime = 0.0f;

	m_DieCoord = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_DieAnimation = Default;

	m_ClipUsed = false;
}

void CNUIElements::SetLifeTime(int LifeTime, float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = time_get() + LifeTime * time_freq();
}

void CNUIElements::SetEndLife(float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
}

void CNUIElements::SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord)
{
	m_DieAnimation = animation_type;
	m_DieCoord = Coord;
}

void CNUIElements::PreRender()
{
	if(m_EndLife && m_EndLifeTime < time_get() && !m_DieProcess)
	{
		m_pColor->Init(vec4(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_pPosLocal->Init(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}

	if(m_pPosLocal->m_AnimTime <= time_get() && time_get() <= m_pPosLocal->m_AnimEndTime)
		m_pPosLocal->Recalculate();
	else if(!m_pPosLocal->m_AnimEnded)
		m_pPosLocal->EndAnimation();

	if(m_pColor->m_AnimTime <= time_get() && time_get() <= m_pColor->m_AnimEndTime)
		m_pColor->Recalculate();
	else if(!m_pColor->m_AnimEnded)
		m_pColor->EndAnimation();

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
}

void CNUIElements::PostRender()
{
	if(m_DieProcess && (m_EndLifeTime + m_EndLifeDur * time_freq() < time_get()))
		m_pControllerNui->RemoveElement(this);

	if (m_pParent && m_pParent->GetClipEnable() && !m_StopClipByParent)
		Graphics()->ClipDisable();
}

vec4 CNUIElements::GetChildPosGlobal()
{
	return m_pPosGlobal + m_pPosLocal->m_Value;
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
	vec4 Pos;
	Pos.x = GetChildPosGlobal().x * m_XScale;
	Pos.y = GetChildPosGlobal().y * m_YScale;
	Pos.w = m_pPosLocal->m_Value.w * m_XScale;
	Pos.h = m_pPosLocal->m_Value.h * m_YScale;

	if (m_pParent && m_pParent->GetClipEnable())
	{
		vec4 ParentPos = m_pParent->GetClipPos();
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
