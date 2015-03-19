#include <base/system.h>
#include <base/math.h>
#include <engine/client.h>
#include "value.h"
#include "nelements.h"

CValue::CValue(CNUIElements *pNUI)
{
	m_pNui = pNUI;
}

void CValue::Init(vec4 Value)
{
	m_Value = Value;
	m_NewValue = Value;
}

void CValue::Init(vec4 Value, float time, ANIMATION_TYPE animation_type)
{
	m_NewValue = Value;
	m_OldValue = m_Value;

	m_AnimEnded = false;
	m_Animation = animation_type;
	m_AnimTime = m_pNui->Client()->GameTick();
	m_AnimEndTime = m_pNui->Client()->GameTick() + (int)round(m_pNui->Client()->GameTickSpeed() * time);
}

void CValue::Recalculate()
{
	double PassedTime = (m_pNui->Client()->GameTick() - m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f);

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
		case EaseIN:			// ������, ������� ��������, ������ ������, ����� �������� - ����� ��������
			time = -cos(time * pi / 2) + 1;
			dbg_msg("EaseIN","%d", time);
			break;
		case EaseOUT:			// ������, ������� ������, ������ ��������, ����� �������� - ����� ����������������
			time = sin(time * pi / 2);
			dbg_msg("EaseOUT","%d", time);
			break;

		case EaseINOUT:			// ������, ������� � ����� ��������, ����� �������� - ����� ��������

		case EaseIN2:			// ������, ������� ��������, ������ ������, ����� �������� - ��������

		case EaseOUT2:			// ������, ������� ������, ������ ��������, ����� �������� - ��������

		case EaseINOUT2:		// ������, ������� � ����� ��������, ����� �������� - ��������

		case EaseIN3:			// ������, ������� ��������, ������ ������, ����� �������� - ������

		case EaseOUT3:			// ������, ������� ������, ������ ��������, ����� �������� - ������

		case EaseINOUT3:		// ������, ������� � ����� ��������, ����� �������� - ������

		case EaseIN4:			// ������, ������� ��������, ������ ������, ����� �������� - ������

		case EaseOUT4:			// ������, ������� ������, ������ ��������, ����� �������� - ������

		case EaseINOUT4:		// ������, ������� � ����� ��������, ����� �������� - ������

		case EaseIN5:			// ������, ������� ��������, ������ ������, ����� �������� - ����� ������

		case EaseOUT5:			// ������, ������� ������, ������ ��������, ����� �������� - ����� ������

		case EaseINOUT5:		// ������, ������� � ����� ��������, ����� �������� - ����� ������

		case EaseINBack:		// ������ c �����������, ������� �����������

		case EaseOUTBack:		// ������ c �����������, ������ �����������

		case EaseINOUTBack:		// ������ c �����������, ������� � ������ �����������

		case EaseINElastic:		// ������, ������ ���������

		case EaseOUTElastic:	// ������, ������� ���������

		case EaseINOUTElastic:	// ������, � ������ ���������

		case EaseINBounce:		// ������, ������� ��������

		case EaseOUTBounce:		// ������, ����� �������

		case EaseINOUTBounce:	// ������, ������� � ������� � ������

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
			dbg_msg("Default","%d", time);
			break;
	}
	return (min + ((max-min) * time));
}