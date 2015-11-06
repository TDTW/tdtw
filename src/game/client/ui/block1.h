#ifndef GAME_CLIENT_UI_NUIBLOCK_H
#define GAME_CLIENT_UI_NUIBLOCK_H

#include "elements.h"

class CBlock : public CNuiElements
{
public:


    CBlock(class CNui *pNui, const char *Name);

	virtual void Render();

	virtual void SetBlock(float RoundCorner, CBlock::CORNER_TYPES Type);

private:
	float m_RoundCorner;
	CORNER_TYPES m_CornerType;
};


#endif //GAME_CLIENT_UI_NUIBLOCK_H
