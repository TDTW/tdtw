#include <base/system.h>
#include <base/math.h>
#include <engine/client.h>
#include "value.h"
#include "nelements.h"

CValue::CValue()
{

}

CValue::CValue(CNUIElements *pNUI)
{
	m_pNui = pNUI;
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
	double PassedTime = (time_get() - m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f);

	m_Value = Animation(m_Animation, m_OldValue, m_NewValue, PassedTime);
}

void CValue::EndAnimation()
{
	m_Value = m_NewValue;
}

vec4 CValue::Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, double time)
{
	switch (anim)
	{
		case EaseIN:			// плавно, вначале медленно, вконце быстро, общая скорость - очень медленно
			time = -cos(time * pi / 2) + 1;
			//dbg_msg("EaseIN","%d", time);
			break;
		case EaseOUT:			// плавно, вначале быстро, вконце медленно, общая скорость - очень медленномедленно
			time = sin(time * pi / 2);
			//dbg_msg("EaseOUT","%d", time);
			break;

		case EaseINOUT:			// плавно, вначале и конце медленно, общая скорость - очень медленно

		case EaseIN2:			// плавно, вначале медленно, вконце быстро, общая скорость - медленно

		case EaseOUT2:			// плавно, вначале быстро, вконце медленно, общая скорость - медленно

		case EaseINOUT2:		// плавно, вначале и конце медленно, общая скорость - медленно

		case EaseIN3:			// плавно, вначале медленно, вконце быстро, общая скорость - средне

		case EaseOUT3:			// плавно, вначале быстро, вконце медленно, общая скорость - средне

		case EaseINOUT3:		// плавно, вначале и конце медленно, общая скорость - средне

		case EaseIN4:			// плавно, вначале медленно, вконце быстро, общая скорость - быстро

		case EaseOUT4:			// плавно, вначале быстро, вконце медленно, общая скорость - быстро

		case EaseINOUT4:		// плавно, вначале и конце медленно, общая скорость - быстро

		case EaseIN5:			// плавно, вначале медленно, вконце быстро, общая скорость - очень быстро

		case EaseOUT5:			// плавно, вначале быстро, вконце медленно, общая скорость - очень быстро

		case EaseINOUT5:		// плавно, вначале и конце медленно, общая скорость - очень быстро

		case EaseINBack:		// плавно c запозданием, вначале запаздываем

		case EaseOUTBack:		// плавно c запозданием, вконце запаздываем

		case EaseINOUTBack:		// плавно c запозданием, вначале и вконце запаздываем

		case EaseINElastic:		// плавно, вконце эластично

		case EaseOUTElastic:	// плавно, вначале эластично

		case EaseINOUTElastic:	// плавно, в центре эластично

		case EaseINBounce:		// плавно, прыгает медленно

		case EaseOUTBounce:		// плавно, сразу прыгает

		case EaseINOUTBounce:	// плавно, прыгает и вначале и вконце

			/*
		case LINEAR:
			time = time;
			break;
		case FADEOUT:
			time=time*time;
			break;
		case FADEIN:
			time=1.0f-pow(time-1.0f, 2);
			break;
		case FADEBOTH:
			time=pow(sinf(time*M_PI*0.5f), 2);
			break;
		case FADEBOTH2:
			time=pow(sinf(time*M_PI*0.5f), 2);
			time=pow(sinf(time*M_PI*0.5f), 2);
			break;
		case INCENTER:
			time=tanf(time*2.0f-1.0f)/3.0f+0.5f;
			break;
		case BALL:
			float d=sinf((time-1.0f/12.0f)*M_PI*6.0f)*0.65f+0.65f;
			time=d+sinf(time*M_PI*0.5f)*(1.0f-d);
			break;
		case STEPS:
			time=(int)(time*7.0f)/7.0f;
			break;
		case LAMP:
			time=sinf(pow(expf(1.25f-time), 2)*4.0f);
			if(time>0.0f)time=1.0f;
			else time=0.0f;
			break;
		case JUMPOUT:
			time=time*time*8.0f/3.0f-time*5.0f/3.0f;
			break;
		case JUMPIN:
			time=time-1.0f;
			time=-time*time*8.0f/3.0f-time*5.0f/3.0f+1.0f;
			break;*/
		default:
			//dbg_msg("Default","%d", time);
			break;
	}
	return (min + ((max-min) * time));
}