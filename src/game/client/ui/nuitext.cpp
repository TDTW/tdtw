#include <engine/textrender.h>
#include <stdarg.h>
#include <stdio.h>
#include <engine/shared/config.h>
#include <game/client/gameclient.h>
CElementText::CElementText(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_Align = ALIGN_LEFT;
	m_TextUpdate = false;
	m_pColorOutline = CValue(this);
}

void CheckProcent2(char *message)
{
	int MessageLength = str_length(message);
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
				case LONG:
					str_format(TempStr, sizeof(TempStr), TempStr, *(long *) m_aArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;

			}
		}
	}
	else
	{
		str_copy(Text, m_UnUpdatedText, sizeof(m_UnUpdatedText));
	}

	vec4 Pos = m_PosLocal.m_Value;
	Pos.x += m_PosGlobal.x;
	Pos.y += m_PosGlobal.y;

	float Height = Pos.h * (g_Config.m_UiScale / 100.0f);

	TextRender()->TextOutlineColor(m_pColorOutline.m_Value.r, m_pColorOutline.m_Value.g, m_pColorOutline.m_Value.b, m_pColorOutline.m_Value.a);
	TextRender()->TextColor(m_Color.m_Value.r, m_Color.m_Value.g, m_Color.m_Value.b, m_Color.m_Value.a);    // TODO: Selected
	if (m_Align == 0)
	{
		float tw = TextRender()->TextWidth(0, Height, Text, -1);
		TextRender()->Text(0, Pos.x + Pos.w / 2 - tw / 2, Pos.y - Height / 10, Height, Text, -1);
	}
	else if (m_Align < 0)
		TextRender()->Text(0, Pos.x, Pos.y - Height / 10, Height, Text, -1);
	else if (m_Align > 0)
	{
		float tw = TextRender()->TextWidth(0, Height, Text, -1);
		TextRender()->Text(0, Pos.x + Pos.w - tw, Pos.y - Height / 10, Height, Text, -1);
	}

	TextRender()->TextColor(1, 1, 1, 1);
	TextRender()->TextOutlineColor(0.0f, 0.0f, 0.0f, 0.3f);
}

void CElementText::ParseTypes(const char *String)
{
	unsigned int MessageLength = str_length(String);

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
				case 'l':
					m_aArgs[Temp]->m_ArgType = LONG;
					m_aArgs[Temp++]->m_EndPos = nLetter + 1;
					break;
			}
			if (String[nLetter + 1] == '0' && String[nLetter + 2] == '.' && String[nLetter + 3] == '2' && String[nLetter + 4] == 'f')
			{
				m_aArgs[Temp]->m_ArgType = FLOAT;
				m_aArgs[Temp++]->m_EndPos = nLetter + 4;
			}
		}
	}
}

void CElementText::SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...)
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
	m_Align = Align;
}

void CElementText::PreRender()
{
	if (m_EndLife && m_EndLifeTime < time_get() && !m_DieProcess)
		m_pColorOutline.Init(vec4(m_pColorOutline.m_Value.r, m_pColorOutline.m_Value.g, m_pColorOutline.m_Value.b, 0.0f), m_EndLifeDur, Default); //TODO animation

	CNUIElements::PreRender();

	if (m_pColorOutline.m_AnimTime <= time_get() && time_get() <= m_pColorOutline.m_AnimEndTime)
		m_pColorOutline.Recalculate();
	else if (!m_pColorOutline.m_AnimEnded)
		m_pColorOutline.EndAnimation();
}