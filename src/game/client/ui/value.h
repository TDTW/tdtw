#ifndef GAME_CLIENT_UI_VALUE_H
#define GAME_CLIENT_UI_VALUE_H

#include <base/vmath.h>
#include "nelements.h"

class CValue
{
public:
	CValue(CNUIElements *pNUI);
	void Init(vec4 value);
	void Init(vec4 value, float time, ANIMATION_TYPE animation_type);

	void Recalculate();
	void EndAnimation();

	bool m_AnimEnded;
	double m_AnimTime;
	double m_AnimEndTime;
	ANIMATION_TYPE m_Animation;
	vec4 m_Value, m_NewValue, m_OldValue;

private:
	CNUIElements *m_pNui;
	vec4 Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, double time);
};

#endif //GAME_CLIENT_UI_VALUE_H
