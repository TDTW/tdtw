#include <stdio.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include "text.h"


CText::CText(class CNui *pNui, char const *Name)
		: CNuiElements(pNui, Name)
{
	m_Align = ALIGN_LEFT;
	m_TextUpdate = false;
	m_apArgs.clear();
	m_pRenderText = NULL;
	m_pTextTemplate = NULL;
}

void CText::ParseTypes(const char *Template, va_list Arguments)
{
	unsigned int MessageLength = (unsigned int) str_length(Template);
	sArg *Temp = NULL;
	for (unsigned int nLetter = 0; nLetter <= MessageLength; nLetter++)
	{
		if (Template[nLetter] == '%' && Template[nLetter + 1])
		{
			switch (Template[nLetter + 1])
			{
				case 'd':
					Temp = new sArg;
					Temp->m_ArgType = INT;
					Temp->m_EndPos = nLetter + 1;
					Temp->m_Args = va_arg(Arguments, void *);
					m_apArgs.add(Temp);
					break;
				case 'f':
					Temp = new sArg;
					Temp->m_ArgType = FLOAT;
					Temp->m_EndPos = nLetter + 1;
					Temp->m_Args = va_arg(Arguments, void *);
					m_apArgs.add(Temp);
					break;
				case 's':
					Temp = new sArg;
					Temp->m_ArgType = STRING;
					Temp->m_EndPos = nLetter + 1;
					Temp->m_Args = va_arg(Arguments, void *);
					m_apArgs.add(Temp);
					break;
				default:
					break;
			}
		}
	}
}

void CText::SetText(bool TextUpdate, CText::TEXT_ALIGN Align, const char *pText, ...)
{
	m_apArgs.clear();
	va_list pArguments;
	va_start(pArguments, pText);
	if (TextUpdate)
	{
		ParseTypes(pText, pArguments);
	}
	else
	{
		int MessageLength = str_length(pText);
		m_pRenderText = new char[512];
		vsprintf(m_pRenderText, pText, pArguments);

		for (unsigned int nLetter = 0; nLetter <= MessageLength; nLetter++)
		{
			if (m_pRenderText[nLetter] == '%')
			{
				m_pRenderText[nLetter] = ' ';
			}
		}
	}
	va_end(pArguments);

	m_TextUpdate = TextUpdate;
	m_pTextTemplate = (char *)pText;
	m_Align = Align;
}

void CText::Render()
{
	char Text[255] = {0};
	if (m_TextUpdate)
	{
		//str_copy(Text, m_pTextTemplate, sizeof(Text));
		for (int i = 0; i < m_apArgs.size(); i++)
		{
			char TempStr[255] = {0};
			if (i > 0 && i != m_apArgs.size())
				mem_copy(TempStr, &m_pTextTemplate[m_apArgs[i - 1]->m_EndPos + 1], m_apArgs[i]->m_EndPos - m_apArgs[i - 1]->m_EndPos);
			else if (i > 0 && i == m_apArgs.size())
				mem_copy(TempStr, &m_pTextTemplate[m_apArgs[i - 1]->m_EndPos + 1], str_length(m_pTextTemplate) - m_apArgs[i - 1]->m_EndPos);
			else
				mem_copy(TempStr, m_pTextTemplate, m_apArgs[i]->m_EndPos + 1);

			switch (m_apArgs[i]->m_ArgType)
			{
				case INT:
					str_format(TempStr, sizeof(TempStr), TempStr, *(int *) m_apArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
				case FLOAT:
					str_format(TempStr, sizeof(TempStr), TempStr, *(float *) m_apArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
				case STRING:
					str_format(TempStr, sizeof(TempStr), TempStr, *(char *) m_apArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;
				case LONG:
					str_format(TempStr, sizeof(TempStr), TempStr, *(long *) m_apArgs[i]->m_Args);
					str_append(Text, TempStr, sizeof(TempStr));
					break;

			}
		}
	}
	else
	{
		str_copy(Text, m_pRenderText, sizeof(Text));

	}

	vec4 Pos = m_pPosLocal->GetValue();
	Pos.x += m_PosGlobal.x;
	Pos.y += m_PosGlobal.y;

	float Height = Pos.h * (g_Config.m_UiScale / 100.0f);
	// TODO: FIX COLOR!!! Sometimes text not rendering
	//TextRender()->TextOutlineColor(m_pColorOutline.m_Value.r, m_pColorOutline.m_Value.g, m_pColorOutline.m_Value.b, m_pColorOutline.m_Value.a);
	//TextRender()->TextColor(m_Color.m_Value.r, m_Color.m_Value.g, m_Color.m_Value.b, m_Color.m_Value.a);
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
