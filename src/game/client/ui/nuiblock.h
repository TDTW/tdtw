#ifndef GAME_CLIENT_UI_NUIBLOCK_H
#define GAME_CLIENT_UI_NUIBLOCK_H

#include "nelements.h"

class CElementBlock : public CNUIElements
{
public:
	CElementBlock(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name);

	virtual void Render();

	virtual void SetBlock(float RoundCorner, CORNER_TYPES Type);

private:
	float m_RoundCorner;
	CORNER_TYPES m_CornerType;

};
#endif //GAME_CLIENT_UI_NUIBLOCK_H
