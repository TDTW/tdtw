#include <engine/client.h>
#include <base/vmath.h>
#include "nuitext.h"
#include "nuiblock.h"
#include "value.h"
#include "nelements.h"
#include <game/client/nui.h>
CNUIElements::CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
{
	m_pName = Name;
	m_pParent = 0;
	m_pClient = pClient;
	m_pControllerNui = pControllerNui;

	m_pPos = new CValue(this);
	m_pColor = new CValue(this);
	m_Renderlevel = NORMAL;

	m_DieProcess = false;
	m_EndLife = false;
	m_EndLifeDur = 0.0f;
	m_EndLifeTime = 0.0f;

	m_DieCoord = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_DieAnimation = Default;
}

void CNUIElements::SetLifeTime(int LifeTime, float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = Client()->GameTick() + LifeTime * Client()->GameTickSpeed();
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
	if(m_EndLife && m_EndLifeTime < Client()->GameTick() && !m_DieProcess)
	{
		m_pColor->Init(vec4(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_pPos->Init(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}

	if(m_pPos->m_AnimTime <= Client()->GameTick() && Client()->GameTick() <= m_pPos->m_AnimEndTime)
		m_pPos->Recalculate();
	else if(!m_pPos->m_AnimEnded)
		m_pPos->EndAnimation();

	if(m_pColor->m_AnimTime <= Client()->GameTick() && Client()->GameTick() <= m_pColor->m_AnimEndTime)
		m_pColor->Recalculate();
	else if(!m_pColor->m_AnimEnded)
		m_pColor->EndAnimation();
}

void CNUIElements::PostRender()
{
	if(m_DieProcess && (m_EndLifeTime + m_EndLifeDur * Client()->GameTickSpeed() < Client()->GameTick()))
		m_pControllerNui->RemoveElement(this);
}