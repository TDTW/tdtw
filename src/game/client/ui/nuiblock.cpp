#include <engine/graphics.h>
#include <game/client/render.h>
#include "nuiblock.h"
#include "nelements.h"
#include "value.h"
CElementBlock::CElementBlock(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_CornerType = CORNER_ALL;
	m_RoundCorner = 0.0f;
}

void CElementBlock::Render()
{
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, m_pColor->m_Value.a);
	RenderTools()->DrawRoundRectExt(m_pPos->m_Value.x, m_pPos->m_Value.y, m_pPos->m_Value.w, m_pPos->m_Value.h, m_RoundCorner, m_CornerType);
	Graphics()->QuadsEnd();
}

void CElementBlock::SetBlock(float RoundCorner, CORNER_TYPES Type)
{
	m_RoundCorner = RoundCorner;
	m_CornerType = Type;
}