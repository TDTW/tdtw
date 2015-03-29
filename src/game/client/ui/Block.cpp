#include <engine/graphics.h>
#include <game/client/render.h>
#include "block.h"

CBlock::CBlock(class CNui *pControllerNui, const char *Name)
    : CNuiElements(pControllerNui, Name)
{
	m_CornerType = CORNER_ALL;
	m_RoundCorner = 0.0f;
}

void CBlock::Render()
{
	vec4 Pos = m_pPosLocal->GetValue();
	Pos.x += m_PosGlobal.x;
	Pos.y += m_PosGlobal.y;
	
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(m_pColor->GetValue().r, m_pColor->GetValue().g, m_pColor->GetValue().b, m_pColor->GetValue().a);
	RenderTools()->DrawRoundRectExt(Pos.x, Pos.y, Pos.w, Pos.h, m_RoundCorner, m_CornerType);
	Graphics()->QuadsEnd();
}

void CBlock::SetBlock(float RoundCorner, CBlock::CORNER_TYPES Type)
{
	m_RoundCorner = RoundCorner;
	m_CornerType = Type;
}


