#ifndef GAME_CLIENT_UI_NUIBLOCK_H
#define GAME_CLIENT_UI_NUIBLOCK_H

#include "elements.h"

class CBlock : public CNuiElements
{
public:
	enum CORNER_TYPES
	{
		CORNER_TL = 1,
		CORNER_TR = 2,
		CORNER_BL = 4,
		CORNER_BR = 8,

		CORNER_T = CORNER_TL | CORNER_TR,
		CORNER_B = CORNER_BL | CORNER_BR,
		CORNER_R = CORNER_TR | CORNER_BR,
		CORNER_L = CORNER_TL | CORNER_BL,

		CORNER_ALL = CORNER_T | CORNER_B
	};

    CBlock(class CNui *pControllerNui, const char *Name);

	virtual void Render();

	virtual void SetBlock(float RoundCorner, CBlock::CORNER_TYPES Type);

private:
	float m_RoundCorner;
	CORNER_TYPES m_CornerType;
};


#endif //GAME_CLIENT_UI_NUIBLOCK_H
