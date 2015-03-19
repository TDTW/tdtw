#include <engine/textrender.h>
#include <stdarg.h>
#include <stdio.h>
#include <engine/shared/config.h>
#include "nuitext.h"
#include "nelements.h"
#include "value.h"
CElementText::CElementText(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name)
		: CNUIElements(pClient, pControllerNui, Name)
{
	m_Height = 0;
	m_Align = ALIGN_LEFT;
	m_TextUpdate = false;
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
