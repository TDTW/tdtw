#ifndef _TDTW_NUIBLOCK_H_
#define _TDTW_NUIBLOCK_H_

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
#endif //_TDTW_NUIBLOCK_H_
