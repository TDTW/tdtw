/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_TEST_H
#define GAME_CLIENT_COMPONENTS_TEST_H
#include <base/vmath.h>
#include <game/client/component.h>

class CTest : public CComponent
{
	enum SPEED
	{
		S_SLOW, S_MID, S_FAST,
	};
	
	enum SIZE
	{
		Z_SMAL, Z_MID, Z_LARG,
	};
	
	struct CItem
	{
		vec2 m_StartPos;
		vec2 m_EndPos;
		float m_StartTime;
		SPEED m_Speed;
		SIZE m_Size;
		int m_Type;
		char m_Text[52];
	};

	enum
	{
		MAX_ITEMS=64,
	};

	CItem m_aItems[MAX_ITEMS];
	int m_NumItems;

	CItem *CreateI();
	void DestroyI(CItem *i);
	
	bool SearchType(int Type);

public:
	CTest();

	void Create(vec2 Pos, int Type);
	virtual void OnRender();
};
#endif
