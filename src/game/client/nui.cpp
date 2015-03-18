#include <engine/shared/config.h>
#include <engine/graphics.h>
#include <engine/client.h>
#include <engine/textrender.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "nui.h"

CNUIElements::CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
{
	m_pName = Name;

	m_pClient = pClient;
	m_pControllerNui = pControllerNui;

	m_pPos = new CValue(this);
	m_pColor = new CValue(this);
	m_Renderlevel = NORMAL;

	m_DieProcess = false;
	m_EndLife = false;
	m_EndLifeDur = 0.0f;
	m_EndLifeTime = 0.0f;

	m_DieCoord = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_DieAnimation = Default;
}

void CNUIElements::SetLifeTime(int LifeTime, float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
	m_EndLifeTime = Client()->GameTick() + LifeTime * Client()->GameTickSpeed();
}

void CNUIElements::SetEndLife(float EndLifeDur)
{
	m_EndLife = true;
	m_EndLifeDur = EndLifeDur;
}

void CNUIElements::SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord)
{
	m_DieAnimation = animation_type;
	m_DieCoord = Coord;
}

void CNUIElements::PreRender()
{
	if(m_EndLife && m_EndLifeTime < Client()->GameTick() && !m_DieProcess)
	{
		m_pColor->Init(vec4(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

		if(m_DieAnimation != Default)
			m_pPos->Init(m_DieCoord, m_EndLifeDur, m_DieAnimation);

		m_DieProcess = true;
	}

	if(m_pPos->m_AnimTime <= Client()->GameTick() && Client()->GameTick() <= m_pPos->m_AnimEndTime)
		m_pPos->Recalculate();
	else if(!m_pPos->m_AnimEnded)
		m_pPos->EndAnimation();

	if(m_pColor->m_AnimTime <= Client()->GameTick() && Client()->GameTick() <= m_pColor->m_AnimEndTime)
		m_pColor->Recalculate();
	else if(!m_pColor->m_AnimEnded)
		m_pColor->EndAnimation();
}

void CNUIElements::PostRender()
{
	if(m_DieProcess && (m_EndLifeTime + m_EndLifeDur * Client()->GameTickSpeed() < Client()->GameTick()))
		m_pControllerNui->RemoveElement(this);
}

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
		case EaseIN:			// плавно, вначале медленно, вконце быстро, общая скорость - очень медленно
			time = -cos(time * pi / 2) + 1;
			dbg_msg("EaseIN","%d", time);
			break;
		case EaseOUT:			// плавно, вначале быстро, вконце медленно, общая скорость - очень медленномедленно
			time = sin(time * pi / 2);
			dbg_msg("EaseOUT","%d", time);
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
			dbg_msg("Default","%d", time);
			break;
	}
	return (min + ((max-min) * time));
}


CControllerNui::CControllerNui(CGameClient *Client)
{
	m_pClient = Client;
}

CNUIElements *CControllerNui::GetElement(ELEMENT_TYPES Type, const char *Name)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (!strcmp(m_aNui[i]->m_pName, Name))
			return m_aNui[i];
	}
	CNUIElements *NewElement = NULL;
	switch (Type)
	{
		case ELEMENT_BLOCK:
			NewElement = new CElementBlock(m_pClient, this, Name);
			break;
		case ELEMENT_TEXT:
			NewElement = new CElementText(m_pClient, this, Name);
			break;
		case ELEMENT_QUAD:
		case ELEMENT_TEE:
		default:
			//NewElement = new CNUIElements(m_pClient, this, Name);
			break;
	}
	m_aNui.add(NewElement);
	return NewElement;
}

void CControllerNui::RemoveElement(CNUIElements *pNui)
{
	for (int i = 0; i < m_aNui.size(); ++i)
	{
		if (m_aNui[i] == pNui)
		{
			m_aNui.remove_index(i);
			break;
		}
	}
}

class IClient *CNUIElements::Client() const
{
	return m_pClient->Client();
}

class IGraphics *CNUIElements::Graphics() const
{
	return m_pClient->Graphics();
}

class ITextRender *CNUIElements::TextRender() const
{
	return m_pClient->TextRender();
}

class CRenderTools *CNUIElements::RenderTools() const
{
	return m_pClient->RenderTools();
}

CElementText::CElementText(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_Height = 0;
	m_Align = ALIGN_LEFT;
	m_TextUpdate = false;
}

void CheckProcent2(char *message)
{
	unsigned int MessageLength = strlen(message);
	for (unsigned int nLetter = 0; nLetter <= MessageLength; nLetter++)
	{
		if (message[nLetter] == '%')
		{
			message[nLetter] = ' ';
		}
	}
}

void CElementText::Render()
{
	char Text[255] = {0};
	if (m_TextUpdate)
	{
		//str_copy(Text, m_pTextTemplate, sizeof(Text));

		for (int i = 0; i < m_aArgs.size(); i++)
		{
			char TempStr[255] = {0};
			if (i > 0 && i != m_aArgs.size())
				mem_copy(TempStr, &m_pTextTemplate[m_aArgs[i - 1]->m_EndPos + 1], m_aArgs[i]->m_EndPos - m_aArgs[i - 1]->m_EndPos);
			else if (i > 0 && i == m_aArgs.size())
				mem_copy(TempStr, &m_pTextTemplate[m_aArgs[i - 1]->m_EndPos + 1], str_length(m_pTextTemplate) - m_aArgs[i - 1]->m_EndPos);
			else
				mem_copy(TempStr, m_pTextTemplate, m_aArgs[i]->m_EndPos + 1);

			switch (m_aArgs[i]->m_ArgType)
			{
				case INT:
					str_format(TempStr, sizeof(TempStr), TempStr, *(int *) m_aArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
				case FLOAT:
					str_format(TempStr, sizeof(TempStr), TempStr, *(float *) m_aArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
				case STRING:
					str_format(TempStr, sizeof(TempStr), TempStr, *(char *) m_aArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
			}
		}
	}
	else
	{
		str_copy(Text, m_UnUpdatedText, sizeof(m_UnUpdatedText));
	}

	TextRender()->TextColor(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, m_pColor->m_Value.a);    // TODO: Selected
	if (m_Align == 0)
	{
		float tw = TextRender()->TextWidth(0, m_Height, Text, -1);
		TextRender()->Text(0, m_pPos->m_Value.x + m_pPos->m_Value.w / 2 - tw / 2, m_pPos->m_Value.y - m_Height / 10, m_Height, Text, -1);
	}
	else if (m_Align < 0)
		TextRender()->Text(0, m_pPos->m_Value.x, m_pPos->m_Value.y - m_Height / 10, m_Height, Text, -1);
	else if (m_Align > 0)
	{
		float tw = TextRender()->TextWidth(0, m_Height, Text, -1);
		TextRender()->Text(0, m_pPos->m_Value.x + m_pPos->m_Value.w - tw, m_pPos->m_Value.y - m_Height / 10, m_Height, Text, -1);
	}
	TextRender()->TextColor(1, 1, 1, 1);
}

void CElementText::ParseTypes(const char *String)
{
	unsigned int MessageLength = strlen(String);

	int Temp = 0;
	for (unsigned int nLetter = 0; nLetter <= MessageLength; nLetter++)
	{
		if (String[nLetter] == '%' && String[nLetter + 1])
		{
			switch (String[nLetter + 1])
			{
				case 'd':
					m_aArgs[Temp]->m_ArgType = INT;
					m_aArgs[Temp++]->m_EndPos = nLetter + 1;
					break;
				case 'f':
					m_aArgs[Temp]->m_ArgType = FLOAT;
					m_aArgs[Temp++]->m_EndPos = nLetter + 1;
					break;
				case 's':
					m_aArgs[Temp]->m_ArgType = STRING;
					m_aArgs[Temp++]->m_EndPos = nLetter + 1;
					break;
			}
		}
	}
}

void CElementText::SetText(bool TextUpdate, float Height, TEXT_ALIGN Align, const char *pText, ...)
{
	m_aArgs.clear();
	va_list pArguments;
	va_start(pArguments, pText);
	if (TextUpdate)
	{
		for (void *Arg = va_arg(pArguments, void *); Arg != NULL; Arg = va_arg(pArguments, void *))
		{
			sArg *Temp = new sArg;
			Temp->m_Args = Arg;
			m_aArgs.add(Temp);
		}
		ParseTypes(pText);
	}
	else
	{
		vsprintf(m_UnUpdatedText, pText, pArguments);
		CheckProcent2(m_UnUpdatedText); // "%" Bug Fix
	}
	va_end(pArguments);

	m_TextUpdate = TextUpdate;
	m_pTextTemplate = pText;
	m_Height = Height * (g_Config.m_UiScale / 100.0f);
	m_Align = Align;
}

CElementBlock::CElementBlock(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_CornerType = CORNER_ALL;
	m_RoundCorner = 0.0f;
}

void CElementBlock::Render()
{
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(m_pColor->m_Value.r, m_pColor->m_Value.g, m_pColor->m_Value.b, m_pColor->m_Value.a);
	RenderTools()->DrawRoundRectExt(m_pPos->m_Value.x, m_pPos->m_Value.y, m_pPos->m_Value.w, m_pPos->m_Value.h, m_RoundCorner, m_CornerType);
	Graphics()->QuadsEnd();
}

void CElementBlock::SetBlock(float RoundCorner, CORNER_TYPES Type)
{
	m_RoundCorner = RoundCorner;
	m_CornerType = Type;
}
