#ifndef GAME_CLIENT_UI_VALUE_H
#define GAME_CLIENT_UI_VALUE_H

#include <base/vmath.h>

enum ANIMATION_TYPE
{
	LINEAR,
	EaseIN,				// плавно, вначале медленно, вконце быстро, общая скорость - очень медленно
	EaseOUT,			// плавно, вначале быстро, вконце медленно, общая скорость - очень медленно
	EaseINOUT,			// плавно, вначале и конце медленно, общая скорость - очень медленно
	EaseIN2,			// плавно, вначале медленно, вконце быстро, общая скорость - медленно
	EaseOUT2,			// плавно, вначале быстро, вконце медленно, общая скорость - медленно
	EaseINOUT2,			// плавно, вначале и конце медленно, общая скорость - медленно
	EaseIN3,			// плавно, вначале медленно, вконце быстро, общая скорость - средне
	EaseOUT3,			// плавно, вначале быстро, вконце медленно, общая скорость - средне
	EaseINOUT3,			// плавно, вначале и конце медленно, общая скорость - средне
	EaseIN4,			// плавно, вначале медленно, вконце быстро, общая скорость - быстро
	EaseOUT4,			// плавно, вначале быстро, вконце медленно, общая скорость - быстро
	EaseINOUT4,			// плавно, вначале и конце медленно, общая скорость - быстро
	EaseIN5,			// плавно, вначале медленно, вконце быстро, общая скорость - очень быстро
	EaseOUT5,			// плавно, вначале быстро, вконце медленно, общая скорость - очень быстро
	EaseINOUT5,			// плавно, вначале и конце медленно, общая скорость - очень быстро
	EaseIN6,            // плавно, вначале медленно, вконце быстро, общая скорость - очень очень быстро
	EaseOUT6,            // плавно, вначале быстро, вконце медленно, общая скорость - очень очень быстро
	EaseINOUT6,            // плавно, вначале и конце медленно, общая скорость - очень очень быстро
	EaseINCirc,            // плавно, вначале медленно, вконце быстро, общая скорость - очень очень быстро
	EaseOUTCirc,        // плавно, вначале быстро, вконце медленно, общая скорость - быстро плавнее предыдущих
	EaseINOUTCirc,        // плавно, вначале и конце медленно, общая скорость - быстро плавнее предыдущих
	EaseINBack,			// плавно c запозданием, вначале запаздываем
	EaseOUTBack,		// плавно c запозданием, вконце запаздываем
	EaseINOUTBack,		// плавно c запозданием, вначале и вконце запаздываем
	EaseINElastic,		// плавно, вконце эластично
	EaseOUTElastic,		// плавно, вначале эластично
	EaseINOUTElastic,	// плавно, в центре эластично
	EaseINBounce,		// плавно, прыгает медленно
	EaseOUTBounce,		// плавно, сразу прыгает
	EaseINOUTBounce,	// плавно, прыгает и вначале и вконце
	Default,
};
class CValue
{
public:
	CValue();
	CValue(class CNUIElements *pNUI);
	void Init(vec4 value);
	void Init(vec4 value, float time, ANIMATION_TYPE animation_type);

	void InitPlus(vec4 value, float time, ANIMATION_TYPE animation_type);

	void Recalculate();
	void EndAnimation();

	bool m_AnimEnded;
	double m_AnimTime;
	double m_AnimEndTime;
	ANIMATION_TYPE m_Animation;
	vec4 m_Value, m_NewValue, m_OldValue;

private:
	class CNUIElements *m_pNui;

	vec4 Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, float time);

	float BounceOut(float time);
};

#endif //GAME_CLIENT_UI_VALUE_H
