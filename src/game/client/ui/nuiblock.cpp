#include <engine/graphics.h>
#include <game/client/render.h>
#include "nuiblock.h"
#include <game/client/gameclient.h>

CElementBlock::CElementBlock(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_CornerType = CORNER_ALL;
	m_RoundCorner = 0.0f;
}

void CElementBlock::Render()
{
	if(m_pParent != NULL)
		m_pPosGlobal = m_pParent->GetChildPosGlobal();
	else
		m_pPosGlobal = vec4(0, 0, 0, 0);

	vec4 Pos = m_pPosLocal->m_Value;
	Pos.x += m_pPosGlobal.x;
	Pos.y += m_pPosGlobal.y;

	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, m_pColor->m_Value.a);
	RenderTools()->DrawRoundRectExt(Pos.x, Pos.y, Pos.w, Pos.h, m_RoundCorner, m_CornerType);
	Graphics()->QuadsEnd();
}

void CElementBlock::SetBlock(float RoundCorner, CORNER_TYPES Type)
{
	m_RoundCorner = RoundCorner;
	m_CornerType = Type;
}