#include "nui.h"

CNUI::CNUI(class CGameClient *pClient, class CControllerNui *pControllerNui)
{
	m_pClient = pClient;
	m_pControllerNui = pControllerNui;

	m_Pos = new CValue(this);
	m_Color = new CValue(this);
	m_Renderlevel = NORMAL;

	m_DieProcess = false;
	m_EndLife = false;
	m_EndLifeDur = 0.0f;
	m_EndLifeTime = 0.0f;

	m_DieCoord = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_DieAnimation = Default;
}

void CNUI::SetLifeTime(int LifeTime, float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = Client()->GameTick() + LifeTime * Client()->GameTickSpeed();
}

void CNUI::SetEndLife(float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
}

void CNUI::SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord)
{
	m_DieAnimation = animation_type;
	m_DieCoord = Coord;
}

void CNUI::Render()
{

	if(m_EndLife && m_EndLifeTime < Client()->GameTick() && !m_DieProcess)
	{
		m_Color->Init(vec4(m_Color->m_Value.r, m_Color->m_Value.g, m_Color->m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_Pos->Init(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}


	if(m_Pos->m_AnimTime < Client()->GameTick() && Client()->GameTick() < m_Pos->m_AnimEndTime)
		m_Pos->Recalculate();
	if(m_Color->m_AnimTime < Client()->GameTick() && Client()->GameTick() < m_Color->m_AnimEndTime)
		m_Color->Recalculate();

	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(m_Color->m_Value.r, m_Color->m_Value.g, m_Color->m_Value.b, m_Color->m_Value.a);
	RenderTools()->DrawRoundRectExt(m_Pos->m_Value.x, m_Pos->m_Value.y, m_Pos->m_Value.w, m_Pos->m_Value.h, 5.0f, CUI::CORNER_ALL);
	Graphics()->QuadsEnd();

	if(m_DieProcess && (m_EndLifeTime + m_EndLifeDur * Client()->GameTickSpeed() < Client()->GameTick()))
		m_pControllerNui->RemoveElement(this);
}

CValue::CValue(CNUI *pNUI)
{
	m_pNui = pNUI;
}

void CValue::Init(vec4 Value)
{
	m_Value = Value;
};

void CValue::Init(vec4 Value, float time, ANIMATION_TYPE animation_type)
{
	m_NewValue = Value;

	m_OldValue = m_Value;

	m_Animation = animation_type;
	m_AnimTime = m_pNui->Client()->GameTick();
	m_AnimEndTime = m_pNui->Client()->GameTick() + (int)round(m_pNui->Client()->GameTickSpeed() * time);
}

void CValue::Recalculate()
{
	double PassedTime = (m_pNui->Client()->GameTick() - m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f);

	m_Value = Animation(m_Animation, m_OldValue, m_NewValue, PassedTime);
}

vec4 CValue::Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, double time)
{
	switch (anim)
	{
		case EaseIN:			// ������, ������� ��������, ������ ������, ����� �������� - ����� ��������
			time = -cos(time * M_PI_2);
			break;
		case EaseOUT:			// ������, ������� ������, ������ ��������, ����� �������� - ����� ����������������
			time = cos(time * M_PI_2);
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
			break;
	}
	return (min + ((max-min) * time));
}


CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
}

CNUI *CControllerNui::GetElement(const char *Name)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if(!strcmp(m_aNui[i]->Name, Name))
			return m_aNui[i]->Element;
	}
	SNUIElements *NewElement = new SNUIElements;
	NewElement->Name = Name;
	NewElement->Element = new CNUI(m_pClient, this);
	m_aNui.add(NewElement);

	return NewElement->Element;
}

void CControllerNui::RemoveElement(CNUI *pNui)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if(m_aNui[i]->Element == pNui)
		{
			m_aNui.remove_index(i);
			break;
		}
	}
}

class IClient *CNUI::Client() const{return m_pClient->Client();}
class IGraphics *CNUI::Graphics() const{return m_pClient->Graphics();}
class CRenderTools *CNUI::RenderTools() const{return m_pClient->RenderTools();}