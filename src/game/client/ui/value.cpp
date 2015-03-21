#include <base/system.h>
#include <base/math.h>
#include "value.h"
#include "nelements.h"

CValue::CValue()
{

}

CValue::CValue(CNUIElements *pNUI)
{
	m_pNui = pNUI;
	m_Value = vec4(1, 1, 1, 1);
}

void CValue::Init(vec4 Value)
{
	m_Value = Value;
	m_NewValue = Value;
	m_AnimEnded = true;
}

void CValue::Init(vec4 Value, float time, ANIMATION_TYPE animation_type)
{
	m_NewValue = Value;
	m_OldValue = m_Value;

	m_AnimEnded = false;
	m_Animation = animation_type;
	m_AnimTime = time_get();
	m_AnimEndTime = time_get() + (int)round(time_freq() * time);
}

void CValue::Recalculate()
{
	float PassedTime = (float) (time_get() - m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f);

	m_Value = Animation(m_Animation, m_OldValue, m_NewValue, PassedTime);
}

void CValue::EndAnimation()
{
	m_Value = m_NewValue;
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
			s = 1.70158f;
			p = time * 0.3f;
			a = (time < 0) ? -1 : 1;

			if (time < 0)
				s = p / 4;
			else
				s = p / (2 * pi) * sinf(a);

			time -= 1;
			time = -(a * powf(2.0f, 10 * time)) * sinf((time * (time + 1) - s) * (2 * pi) / p);
			break;
		case EaseOUTElastic:	// плавно, вначале эластично
			s = 1.70158f;
			p = time * 0.3f;
			a = (time < 0) ? -1 : 1;

			if (time < 0)
				s = p / 4;
			else
				s = p / (2 * pi) * sinf(a);

			time = a * powf(2.0f, -10 * time) * sinf((time * time - s) * (2 * pi) / p) + 1;
			break;
		case EaseINOUTElastic:	// плавно, в центре эластично
			d = time;
			time *= 2;

			s = 1.70158f;
			p = d * (0.3f * 1.5f);
			a = (time < 0) ? -1 : 1;

			if (time < 0)
				s = p / 4;
			else
				s = p / (2 * pi) * sinf(a);

			if (time < 1)
			{
				time -= 1;
				time = -0.5f * (a * powf(2.0f, 10 * time) * sinf((time * d - s) * (2 * pi) / p));
			}
			else
			{
				time -= 1;
				time = a * powf(2.0f, -10 * time) * sinf((time * d - s) * (2 * pi) / p) * 0.5f + 1;
			}

			break;

		case EaseINBounce:        // плавно, прыгает медленно
			time = 1.0f - time;
			if (time < 1 / 2.75f)
			{
				time = 1 - 7.5625f * time * time;
			}
			else if (time < 2 / 2.75f)
			{
				time -= 1.5f / 2.75f;
				time = 1 - 7.5625f * time * time - 0.75f;
			}
			else if (time < 2.5 / 2.75f)
			{
				time -= 2.25f / 2.75f;
				time = 1 - 7.5625f * time * time - 0.9375f;
			}
			else
			{
				time -= 2.625f / 2.75f;
				time = 1 - 7.5625f * time * time - 0.984375f;
			}
			break;
		case EaseOUTBounce:		// плавно, сразу прыгает
			if (time < 1 / 2.75f)
			{
				time = 7.5625f * time * time;
			}
			else if (time < 2 / 2.75f)
			{
				time -= 1.5f / 2.75f;
				time = 7.5625f * time * time + 0.75f;
			}
			else if (time < 2.5 / 2.75f)
			{
				time -= 2.25f / 2.75f;
				time = 7.5625f * time * time + 0.9375f;
			}
			else
			{
				time -= 2.625f / 2.75f;
				time = 7.5625f * time * time + 0.984375f;
			}
			break;
		case EaseINOUTBounce:	// плавно, прыгает и вначале и вконце
			d = time;
			time *= 2;
			if (time < 1)
			{
				time = 1.0f - time;
				if (time < 1 / 2.75f)
				{
					time = 1 - 7.5625f * time * time;
				}
				else if (time < 2 / 2.75f)
				{
					time -= 1.5f / 2.75f;
					time = 1 - 7.5625f * time * time - 0.75f;
				}
				else if (time < 2.5 / 2.75f)
				{
					time -= 2.25f / 2.75f;
					time = 1 - 7.5625f * time * time - 0.9375f;
				}
				else
				{
					time -= 2.625f / 2.75f;
					time = 1 - 7.5625f * time * time - 0.984375f;
				}
				time *= 0.5f;
			}
			else
			{
				time -= d;
				if (time < 1 / 2.75f)
				{
					time = 7.5625f * time * time;
				}
				else if (time < 2 / 2.75f)
				{
					time -= 1.5f / 2.75f;
					time = 7.5625f * time * time + 0.75f;
				}
				else if (time < 2.5 / 2.75f)
				{
					time -= 2.25f / 2.75f;
					time = 7.5625f * time * time + 0.9375f;
				}
				else
				{
					time -= 2.625f / 2.75f;
					time = 7.5625f * time * time + 0.984375f;
				}
				time = time * 0.5f + 0.5f;
			}
			break;

		default:
			//dbg_msg("Default","%d", time);
			break;
	}
	return (min + ((max-min) * time));
}