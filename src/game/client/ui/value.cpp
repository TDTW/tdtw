#include <base/system.h>
#include <base/math.h>
#include "value.h"
#include "elements.h"

CValue::CValue()
{
	m_BackAnimation = false;
	m_Value = vec4(1, 1, 1, 1);
}

void CValue::Init(vec4 Value)
{
	m_Value = Value;
	m_NewValue = Value;
	m_OldValue = Value;
	m_AnimEnded = true;
}

void CValue::Init(vec4 Value, float time, ANIMATION_TYPE AnimationType, bool BackAnimation)
{
	m_NewValue = Value;
	m_OldValue = m_Value;

	m_AnimEnded = false;
	m_Animation = AnimationType;
	m_AnimTime = time_get();
	m_AnimEndTime = time_get() + (int)round(time_freq() * time);

	m_BackAnimation = BackAnimation;
}

void CValue::InitPlus(vec4 Value, float time, ANIMATION_TYPE AnimationType, bool BackAnimation)
{
	Init(m_NewValue + Value, time, AnimationType, BackAnimation);
}

void CValue::Recalculate()
{
	float PassedTime = (float)((time_get() - m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f));

	m_Value = Animation(m_Animation, m_OldValue, m_NewValue, PassedTime);
}

void CValue::EndAnimation()
{
	m_AnimEnded = true;
	m_Value = m_NewValue;

	if (m_BackAnimation)
		Init(m_OldValue, (float)((m_AnimEndTime - m_AnimTime) / time_freq()), m_Animation, false);
}

vec4 CValue::Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, float time)
{
	float s = 0.0f, a = 0.0f, p = 0.0f, d = 0.0f;
	switch (anim)
	{
		case EaseIN:			// плавно, вначале медленно, вконце быстро, общая скорость - очень медленно
			time = -cosf(time * pi / 2) + 1;
			break;
		case EaseOUT:			// плавно, вначале быстро, вконце медленно, общая скорость - очень медленномедленно
			time = sinf(time * pi / 2);
			break;
		case EaseINOUT:			// плавно, вначале и конце медленно, общая скорость - очень медленно
			time = -0.5f * (cosf(pi * time) - 1);
			break;

		case EaseIN2:			// плавно, вначале медленно, вконце быстро, общая скорость - медленно
			time = time * time;
			break;
		case EaseOUT2:			// плавно, вначале быстро, вконце медленно, общая скорость - медленно
			time = -time * (time - 2);
			break;
		case EaseINOUT2:		// плавно, вначале и конце медленно, общая скорость - медленно
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * time * time;
			}
			else
			{
				time -= 1;
				time = -0.5f * (time * (time - 2) - 1);
			}
			break;

		case EaseIN3:			// плавно, вначале медленно, вконце быстро, общая скорость - средне
			time = time * time * time;
			break;
		case EaseOUT3:			// плавно, вначале быстро, вконце медленно, общая скорость - средне
			time -= 1;
			time = time * time * time + 1;
			break;
		case EaseINOUT3:		// плавно, вначале и конце медленно, общая скорость - средне
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * time * time * time;
			}
			else
			{
				time -= 2;
				time = 0.5f * (time * time * time + 2);
			}
			break;

		case EaseIN4:			// плавно, вначале медленно, вконце быстро, общая скорость - быстро
			time = time * time * time * time;
			break;
		case EaseOUT4:			// плавно, вначале быстро, вконце медленно, общая скорость - быстро
			time -= 1;
			time = -(time * time * time * time - 1);
			break;
		case EaseINOUT4:		// плавно, вначале и конце медленно, общая скорость - быстро
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * time * time * time * time;
			}
			else
			{
				time -= 2;
				time = -0.5f * (time * time * time * time - 2);
			}
			break;

		case EaseIN5:			// плавно, вначале медленно, вконце быстро, общая скорость - очень быстро
			time = time * time * time * time * time;
			break;
		case EaseOUT5:			// плавно, вначале быстро, вконце медленно, общая скорость - очень быстро
			time -= 1;
			time = time * time * time * time * time + 1;
			break;
		case EaseINOUT5:		// плавно, вначале и конце медленно, общая скорость - очень быстро
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * (time * time * time * time * time);
			}
			else
			{
				time -= 2;
				time = 0.5f * (time * time * time * time * time + 2);
			}
			break;

		case EaseIN6:            // плавно, вначале медленно, вконце быстро, общая скорость - очень очень быстро
			time = powf(2.0f, 10 * (time - 1));
			break;
		case EaseOUT6:            // плавно, вначале быстро, вконце медленно, общая скорость - очень очень быстро
			time = -powf(2.0f, -10 * time) + 1;
			break;
		case EaseINOUT6:        // плавно, вначале и конце медленно, общая скорость - очень очень быстро
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * powf(2.0f, 10 * (time - 1));
			}
			else
			{
				time -= 1;
				time = 0.5f * (-powf(2.0f, -10 * time) + 2);
			}
			break;

		case EaseINCirc:        // плавно, вначале медленно, вконце быстро, общая скорость - очень очень быстро
			time = -(sqrtf(1 - time * time) - 1);
			break;
		case EaseOUTCirc:        // плавно, вначале быстро, вконце медленно, общая скорость - быстро плавнее предыдущих
			time -= 1;
			time = sqrtf(1 - time * time);
			break;
		case EaseINOUTCirc:        // плавно, вначале и конце медленно, общая скорость - быстро плавнее предыдущих
			time *= 2;
			if (time < 1)
			{
				time = -0.5f * (sqrtf(1 - time * time) - 1);
			}
			else
			{
				time -= 2;
				time = 0.5f * (sqrtf(1 - time * time) + 1);
			}
			break;

		case EaseINBack:		// плавно c запозданием, вначале запаздываем
			s = 1.70158f;
			time = time * time * ((s + 1) * time - s);
			break;
		case EaseOUTBack:		// плавно c запозданием, вконце запаздываем
			s = 1.70158f;
			time -= 1;
			time = time * time * ((s + 1) * time + s) + 1;
			break;
		case EaseINOUTBack:		// плавно c запозданием, вначале и вконце запаздываем
			s = 1.70158f * 1.525f;
			time *= 2;
			if (time < 1)
			{
				time = 0.5f * (time * time * ((s + 1) * time - s));
			}
			else
			{
				time -= 2;
				time = 0.5f * (time * time * ((s + 1) * time + s) + 2);
			}
			break;

		case EaseINElastic:		// плавно, вконце эластично
			time = sinf(13 * pi / 2.0f * time) * powf(2.0f, 10 * (time - 1));
			break;
		case EaseOUTElastic:	// плавно, вначале эластично
			time = sinf(-13 * pi / 2.0f * (time + 1)) * powf(2.0f, -10 * time) + 1;
			break;
		case EaseINOUTElastic:	// плавно, в центре эластично
			if (time < 0.5)
			{
				time = 0.5f * sinf(13 * pi / 2.0f * (2 * time)) * powf(2.0f, 10 * ((2.0f * time) - 1));
			}
			else
			{
				time = 0.5f * (sinf(-13 * pi / 2.0f * ((2 * time - 1) + 1)) * powf(2.0f, -10 * (2.0f * time - 1)) + 2);
			}

			break;

		case EaseINBounce:        // плавно, прыгает медленно
			time = 1 - BounceOut(1 - time);
			break;
		case EaseOUTBounce:		// плавно, сразу прыгает
			time = BounceOut(time);
			break;
		case EaseINOUTBounce:	// плавно, прыгает и вначале и вконце
			if (time < 0.5f)
				time = 0.5f * (1 - BounceOut(1 - time * 2));
			else
				time = 0.5f * BounceOut(time * 2 - 1) + 0.5f;
			break;

		default:
			//dbg_msg("Default","%d", time);
			break;
	}
	return (min + ((max-min) * time));
}

float CValue::BounceOut(float time)
{
	if (time < 4 / 11.0f)
	{
		time = (121 * time * time) / 16.0f;
	}
	else if (time < 8 / 11.0f)
	{
		time = (363 / 40.0f * time * time) - (99 / 10.0f * time) + 17 / 5.0f;
	}
	else if (time < 9 / 10.0f)
	{
		time = (4356 / 361.0f * time * time) - (35442 / 1805.0f * time) + 16061 / 1805.0f;
	}
	else
	{
		time = (54 / 5.0f * time * time) - (513 / 25.0f * time) + 268 / 25.0f;
	}
	return time;
}

void CValue::PreRender()
{
	if(m_AnimTime <= time_get() && time_get() <= m_AnimEndTime)
		Recalculate();
	else if(!m_AnimEnded)
	{
		EndAnimation();
		//if(m_AnimEnd)
		//	m_AnimEnd(this, m_pAnimArg);
	}
}
