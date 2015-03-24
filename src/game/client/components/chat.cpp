/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <engine/engine.h>
#include <engine/graphics.h>
#include <engine/textrender.h>
#include <engine/keys.h>
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/client/gameclient.h>

#include <game/client/components/scoreboard.h>
#include <game/client/components/sounds.h>
#include <game/client/animstate.h>
#include <game/localization.h>

#include "chat.h"
#include "binds.h"


CChat::CChat()
{
	OnReset();
}

void CChat::OnReset()
{
	for(int i = 0; i < MAX_LINES; i++)
	{
		m_aLines[i].m_Time = 0;
		m_aLines[i].m_aText[0] = 0;
		m_aLines[i].m_aName[0] = 0;
		m_aLines[i].m_Blend = 0.0f;
		m_aLines[i].m_Tee = CTeeRenderInfo();
	}

	m_Show = false;
	m_InputUpdate = false;
	m_ChatStringOffset = 0;
	m_CompletionChosen = -1;
	m_aCompletionBuffer[0] = 0;
	m_PlaceholderOffset = 0;
	m_PlaceholderLength = 0;
	m_pHistoryEntry = 0x0;
	m_PendingChatCounter = 0;
	m_LastChatSend = 0;
	ChatY = 0;

	for(int i = 0; i < CHAT_NUM; ++i)
		m_aLastSoundPlayed[i] = 0;
}

void CChat::OnRelease()
{
	m_Show = false;
}

bool CChat::OnMouseMove(float x, float y)
{
	if(!m_Show && m_Mode == MODE_NONE)
		return false;

	UI()->ConvertMouseMove(&x, &y);
	m_MousePos.x += x;
	m_MousePos.y += y;
	//if(m_MousePos.x < 0) m_MousePos.x = 0;
	//if(m_MousePos.y < 0) m_MousePos.y = 0;
	//if(m_MousePos.x > Graphics()->ScreenWidth()) m_MousePos.x = Graphics()->ScreenWidth();
	//if(m_MousePos.y > Graphics()->ScreenHeight()) m_MousePos.y = Graphics()->ScreenHeight();

	return true;
}

void CChat::OnStateChange(int NewState, int OldState)
{
	if(OldState <= IClient::STATE_CONNECTING)
	{
		m_Mode = MODE_NONE;
		for(int i = 0; i < MAX_LINES; i++)
			m_aLines[i].m_Time = 0;
		m_CurrentLine = 0;
	}
}

void CChat::ConSay(IConsole::IResult *pResult, void *pUserData)
{
	((CChat*)pUserData)->Say(0, pResult->GetString(0));
}

void CChat::ConSayTeam(IConsole::IResult *pResult, void *pUserData)
{
	((CChat*)pUserData)->Say(1, pResult->GetString(0));
}

void CChat::ConChat(IConsole::IResult *pResult, void *pUserData)
{
	const char *pMode = pResult->GetString(0);
	if(str_comp(pMode, "all") == 0)
		((CChat*)pUserData)->EnableMode(0);
	else if(str_comp(pMode, "team") == 0)
		((CChat*)pUserData)->EnableMode(1);
	else
		((CChat*)pUserData)->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "console", "expected all or team as mode");
}

void CChat::ConShowChat(IConsole::IResult *pResult, void *pUserData)
{
	((CChat *)pUserData)->m_Show = pResult->GetInteger(0) != 0;
}

void CChat::OnConsoleInit()
{
	Console()->Register("say", "r", CFGFLAG_CLIENT, ConSay, this, "Say in chat");
	Console()->Register("say_team", "r", CFGFLAG_CLIENT, ConSayTeam, this, "Say in team chat");
	Console()->Register("chat", "s", CFGFLAG_CLIENT, ConChat, this, "Enable chat with all/team mode");
	Console()->Register("+show_chat", "", CFGFLAG_CLIENT, ConShowChat, this, "Show chat");
}

bool CChat::OnInput(IInput::CEvent Event)
{
	if(m_Mode == MODE_NONE)
	{
		if(m_Show)
		{
			if(Event.m_Flags&IInput::FLAG_RELEASE && str_comp(m_pClient->m_pBinds->Get(Event.m_Key, false), "+show_chat") == 0)
				m_Show = false;
			else
				return true;
		}
		else
			return false;
	}

	if(Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key == KEY_ESCAPE)
	{
		m_Mode = MODE_NONE;
		m_pClient->OnRelease();
	}
	else if(Event.m_Flags&IInput::FLAG_PRESS && (Event.m_Key == KEY_RETURN || Event.m_Key == KEY_KP_ENTER))
	{
		if(m_Input.GetString()[0])
		{
			bool AddEntry = false;

			if(m_LastChatSend+time_freq() < time_get())
			{
				Say(m_Mode == MODE_ALL ? 0 : 1, m_Input.GetString());
				AddEntry = true;
			}
			else if(m_PendingChatCounter < 3)
			{
				++m_PendingChatCounter;
				AddEntry = true;
			}

			if(AddEntry)
			{
				CHistoryEntry *pEntry = m_History.Allocate(sizeof(CHistoryEntry)+m_Input.GetLength());
				pEntry->m_Team = m_Mode == MODE_ALL ? 0 : 1;
				mem_copy(pEntry->m_aText, m_Input.GetString(), m_Input.GetLength()+1);
			}
		}
		m_pHistoryEntry = 0x0;
		m_Mode = MODE_NONE;
		m_pClient->OnRelease();
	}
	if(Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key == KEY_TAB)
	{
		// fill the completion buffer
		if(m_CompletionChosen < 0)
		{
			const char *pCursor = m_Input.GetString()+m_Input.GetCursorOffset();
			for(int Count = 0; Count < m_Input.GetCursorOffset() && *(pCursor-1) != ' '; --pCursor, ++Count);
			m_PlaceholderOffset = pCursor-m_Input.GetString();

			for(m_PlaceholderLength = 0; *pCursor && *pCursor != ' '; ++pCursor)
				++m_PlaceholderLength;

			str_copy(m_aCompletionBuffer, m_Input.GetString()+m_PlaceholderOffset, min(static_cast<int>(sizeof(m_aCompletionBuffer)), m_PlaceholderLength+1));
		}

		// find next possible name
		const char *pCompletionString = 0;
		m_CompletionChosen = (m_CompletionChosen+1)%(2*MAX_CLIENTS);
		for(int i = 0; i < 2*MAX_CLIENTS; ++i)
		{
			int SearchType = ((m_CompletionChosen+i)%(2*MAX_CLIENTS))/MAX_CLIENTS;
			int Index = (m_CompletionChosen+i)%MAX_CLIENTS;
			if(!m_pClient->m_Snap.m_paPlayerInfos[Index])
				continue;

			bool Found = false;
			if(SearchType == 1)
			{
				if(str_comp_nocase_num(m_pClient->m_aClients[Index].m_aName, m_aCompletionBuffer, str_length(m_aCompletionBuffer)) &&
					str_find_nocase(m_pClient->m_aClients[Index].m_aName, m_aCompletionBuffer))
					Found = true;
			}
			else if(!str_comp_nocase_num(m_pClient->m_aClients[Index].m_aName, m_aCompletionBuffer, str_length(m_aCompletionBuffer)))
				Found = true;

			if(Found)
			{
				pCompletionString = m_pClient->m_aClients[Index].m_aName;
				m_CompletionChosen = Index+SearchType*MAX_CLIENTS;
				break;
			}
		}

		// insert the name
		if(pCompletionString)
		{
			char aBuf[256];
			// add part before the name
			str_copy(aBuf, m_Input.GetString(), min(static_cast<int>(sizeof(aBuf)), m_PlaceholderOffset+1));

			// add the name
			str_append(aBuf, pCompletionString, sizeof(aBuf));

			// add seperator
			const char *pSeparator = "";
			if(*(m_Input.GetString()+m_PlaceholderOffset+m_PlaceholderLength) != ' ')
				pSeparator = m_PlaceholderOffset == 0 ? ", " : " ";
			else if(m_PlaceholderOffset == 0)
				pSeparator = ",";
			if(*pSeparator)
				str_append(aBuf, pSeparator, sizeof(aBuf));

			// add part after the name
			str_append(aBuf, m_Input.GetString()+m_PlaceholderOffset+m_PlaceholderLength, sizeof(aBuf));

			m_PlaceholderLength = str_length(pSeparator)+str_length(pCompletionString);
			m_OldChatStringLength = m_Input.GetLength();
			m_Input.Set(aBuf);
			m_Input.SetCursorOffset(m_PlaceholderOffset+m_PlaceholderLength);
			m_InputUpdate = true;
		}
	}
	else
	{
		// reset name completion process
		if(Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key != KEY_TAB)
			m_CompletionChosen = -1;

		m_OldChatStringLength = m_Input.GetLength();
		m_Input.ProcessInput(Event);
		m_InputUpdate = true;
	}
	if(Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key == KEY_UP)
	{
		if(m_pHistoryEntry)
		{
			CHistoryEntry *pTest = m_History.Prev(m_pHistoryEntry);

			if(pTest)
				m_pHistoryEntry = pTest;
		}
		else
			m_pHistoryEntry = m_History.Last();

		if(m_pHistoryEntry)
			m_Input.Set(m_pHistoryEntry->m_aText);
	}
	else if (Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key == KEY_DOWN)
	{
		if(m_pHistoryEntry)
			m_pHistoryEntry = m_History.Next(m_pHistoryEntry);

		if (m_pHistoryEntry)
			m_Input.Set(m_pHistoryEntry->m_aText);
		else
			m_Input.Clear();
	}

	return true;
}


void CChat::EnableMode(int Team)
{
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	if(m_Mode == MODE_NONE)
	{
		if(Team)
			m_Mode = MODE_TEAM;
		else
			m_Mode = MODE_ALL;

		m_Input.Clear();
		Input()->ClearEvents();
		m_CompletionChosen = -1;
	}
}

void CChat::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		AddLine(pMsg->m_ClientID, pMsg->m_Team, pMsg->m_pMessage);
	}
}

void CChat::AddLine(int ClientID, int Team, const char *pLine)
{
	if(*pLine == 0 || (ClientID != -1 && (m_pClient->m_aClients[ClientID].m_aName[0] == '\0' || // unknown client
		m_pClient->m_aClients[ClientID].m_ChatIgnore ||
		(m_pClient->m_Snap.m_LocalClientID != ClientID && g_Config.m_ClShowChatFriends && !m_pClient->m_aClients[ClientID].m_Friend))))
		return;

	// trim right and set maximum length to 128 utf8-characters
	int Length = 0;
	const char *pStr = pLine;
	const char *pEnd = 0;
	while(*pStr)
 	{
		const char *pStrOld = pStr;
		int Code = str_utf8_decode(&pStr);

		// check if unicode is not empty
		if(Code > 0x20 && Code != 0xA0 && Code != 0x034F && (Code < 0x2000 || Code > 0x200F) && (Code < 0x2028 || Code > 0x202F) &&
			(Code < 0x205F || Code > 0x2064) && (Code < 0x206A || Code > 0x206F) && (Code < 0xFE00 || Code > 0xFE0F) &&
			Code != 0xFEFF && (Code < 0xFFF9 || Code > 0xFFFC))
		{
			pEnd = 0;
		}
		else if(pEnd == 0)
			pEnd = pStrOld;

		if(++Length >= 127)
		{
			*(const_cast<char *>(pStr)) = 0;
			break;
		}
 	}
	if(pEnd != 0)
		*(const_cast<char *>(pEnd)) = 0;

	bool Highlighted = false;
	char *p = const_cast<char*>(pLine);
	while(*p)
	{
		Highlighted = false;
		pLine = p;
		// find line seperator and strip multiline
		while(*p)
		{
			if(*p++ == '\n')
			{
				*(p-1) = 0;
				break;
			}
		}

		m_CurrentLine = (m_CurrentLine+1)%MAX_LINES;
		m_aLines[m_CurrentLine].m_Time = time_get();
		m_aLines[m_CurrentLine].m_YNew[0] = -1.0f;
		m_aLines[m_CurrentLine].m_YNew[1] = -1.0f;
		m_aLines[m_CurrentLine].m_YOld[0] = -1.0f;
		m_aLines[m_CurrentLine].m_YOld[1] = -1.0f;
		m_aLines[m_CurrentLine].m_ClientID = ClientID;
		m_aLines[m_CurrentLine].m_Team = Team;
		m_aLines[m_CurrentLine].m_NameColor = -2;
		m_aLines[m_CurrentLine].m_Blend = 0.0f;
		m_aLines[m_CurrentLine].m_Tee = CTeeRenderInfo();
		

		// check for highlighted name
		const char *pHL = str_find_nocase(pLine, m_pClient->m_aClients[m_pClient->m_Snap.m_LocalClientID].m_aName);
		if(pHL)
		{
			int Length = str_length(m_pClient->m_aClients[m_pClient->m_Snap.m_LocalClientID].m_aName);
			if((pLine == pHL || pHL[-1] == ' ') /*&& (pHL[Length] == 0 || pHL[Length] == ' ' || (pHL[Length] == ':' && pHL[Length+1] == ' '))*/)
				Highlighted = true;
		}
		m_aLines[m_CurrentLine].m_Highlighted =  Highlighted;

		if(ClientID == -1) // server message
		{
			str_copy(m_aLines[m_CurrentLine].m_aName, "*** ", sizeof(m_aLines[m_CurrentLine].m_aName));
			str_format(m_aLines[m_CurrentLine].m_aText, sizeof(m_aLines[m_CurrentLine].m_aText), "%s", pLine);
		}
		else
		{
			if(m_pClient->m_aClients[ClientID].m_Team == TEAM_SPECTATORS)
				m_aLines[m_CurrentLine].m_NameColor = TEAM_SPECTATORS;

			if(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags&GAMEFLAG_TEAMS)
			{
				if(m_pClient->m_aClients[ClientID].m_Team == TEAM_RED)
					m_aLines[m_CurrentLine].m_NameColor = TEAM_RED;
				else if(m_pClient->m_aClients[ClientID].m_Team == TEAM_BLUE)
					m_aLines[m_CurrentLine].m_NameColor = TEAM_BLUE;
			}
			
			m_aLines[m_CurrentLine].m_Tee = m_pClient->m_aClients[ClientID].m_RenderInfo;

			str_copy(m_aLines[m_CurrentLine].m_aName, m_pClient->m_aClients[ClientID].m_aName, sizeof(m_aLines[m_CurrentLine].m_aName));
			str_format(m_aLines[m_CurrentLine].m_aText, sizeof(m_aLines[m_CurrentLine].m_aText), ": %s", pLine);
		}

		char aBuf[1024];
		str_format(aBuf, sizeof(aBuf), "%s%s", m_aLines[m_CurrentLine].m_aName, m_aLines[m_CurrentLine].m_aText);
		Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, m_aLines[m_CurrentLine].m_Team?"teamchat":"chat", aBuf);
	}

	// play sound
	int64 Now = time_get();
	if(ClientID == -1)
	{
		if(Now-m_aLastSoundPlayed[CHAT_SERVER] >= time_freq()*3/10)
		{
			m_pClient->m_pSounds->Play(CSounds::CHN_GUI, SOUND_CHAT_SERVER, 0);
			m_aLastSoundPlayed[CHAT_SERVER] = Now;
		}
	}
	else if(Highlighted)
	{
		if(Now-m_aLastSoundPlayed[CHAT_HIGHLIGHT] >= time_freq()*3/10)
		{
			m_pClient->m_pSounds->Play(CSounds::CHN_GUI, SOUND_CHAT_HIGHLIGHT, 0);
			m_aLastSoundPlayed[CHAT_HIGHLIGHT] = Now;
		}
	}
	else
	{
		if(Now-m_aLastSoundPlayed[CHAT_CLIENT] >= time_freq()*3/10)
		{
			m_pClient->m_pSounds->Play(CSounds::CHN_GUI, SOUND_CHAT_CLIENT, 0);
			m_aLastSoundPlayed[CHAT_CLIENT] = Now;
		}
	}
}

float *CChat::ButtonFade(const void *pID, const void *pValueFade, float Seconds, int Checked)
{
	float *pFade = (float*)pValueFade;

	if(UI()->ActiveItem() == pID)
	{
		*pFade -= Client()->RenderFrameTime()*3;
		if(*pFade < 0.35f*Seconds)
			*pFade = 0.35f*Seconds;
	}
	else if(UI()->HotItem() == pID)
	{
		*pFade += Client()->RenderFrameTime()*2;
		if(*pFade > Seconds)
			*pFade = Seconds;
	}
	else if(Checked)
	{
		*pFade += Client()->RenderFrameTime()*2;
		if(*pFade > 0.85f*Seconds)
			*pFade = 0.85f*Seconds;
	}
	else if(*pFade > 0.0f)
	{
		*pFade -= Client()->RenderFrameTime();
		if(*pFade < 0.0f)
			*pFade = 0.0f;
	}
	return pFade;
}

float *CChat::ButtonFade(const void *pID, const void *pValueFade, const CUIRect *pRect, float Seconds)
{
	float *pFade = (float*)pValueFade;

	if(UI()->MouseInside(pRect) || UI()->ActiveItem() == pID)
	{
		*pFade += Client()->RenderFrameTime()*2;
		if(*pFade > Seconds)
			*pFade = Seconds;
	}
	else if(*pFade > 0.0f)
	{
		*pFade -= Client()->RenderFrameTime();
		if(*pFade < 0.0f)
			*pFade = 0.0f;
	}
	return pFade;
}

float CChat::DoScrollbarV(const void *pID, const float *pFade, const CUIRect *pRect, float Current, int SizeSlider)
{
	CUIRect Handle;
	static float OffsetY;
	if(SizeSlider < 33)
		SizeSlider = 33;
	pRect->HSplitTop(SizeSlider, &Handle, 0);

	Handle.y += (pRect->h-Handle.h)*Current;

	// logic
	float ReturnValue = Current;
	int Inside = UI()->MouseInside(&Handle);

	if(UI()->ActiveItem() == pID)
	{
		if(!UI()->MouseButton(0))
			UI()->SetActiveItem(0);

		float Min = pRect->y;
		float Max = pRect->h-Handle.h;
		float Cur = UI()->MouseY()-OffsetY;
		ReturnValue = (Cur-Min)/Max;
		if(ReturnValue < 0.0f) ReturnValue = 0.0f;
		if(ReturnValue > 1.0f) ReturnValue = 1.0f;
	}
	else if(UI()->HotItem() == pID)
	{
		if(UI()->MouseButton(0))
		{
			UI()->SetActiveItem(pID);
			OffsetY = UI()->MouseY()-Handle.y;
		}
	}

	if(Inside)
		UI()->SetHotItem(pID);
		
	float *pRailFade = ButtonFade(pID, &pFade[0], pRect, 0.6f);
	float RailFadeVal = *pRailFade/0.6f;
	
	vec4 RailColor = mix(vec4(1.0f, 1.0f, 1.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 0.25f), RailFadeVal);
		 				
	// render
	CUIRect Rail;
	pRect->VMargin(5.0f, &Rail);
	RenderTools()->DrawUIRect(&Rail, RailColor, 0, 0.0f);

	CUIRect Slider = Handle;
	Slider.w = Rail.x-Slider.x;
	RenderTools()->DrawUIRect(&Slider, RailColor, CUI::CORNER_L, 2.5f);
	Slider.x = Rail.x+Rail.w;
	RenderTools()->DrawUIRect(&Slider, RailColor, CUI::CORNER_R, 2.5f);

	float *pButtFade = ButtonFade(pID, &pFade[1], 0.6f);
	float ButtFadeVal = *pButtFade/0.6f;
	
	vec4 ButtColor = mix(vec4(1.0f, 1.0f, 1.0f, 0.25f), vec4(1.0f, 1.0f, 1.0f, 0.75f), ButtFadeVal);
	
	Slider = Handle;
	Slider.Margin(5.0f, &Slider);
	RenderTools()->DrawUIRect(&Slider, ButtColor, CUI::CORNER_ALL, 2.5f);

	return ReturnValue;
}

void CChat::OnRenderNew()
{
	// send pending chat messages
	if(m_PendingChatCounter > 0 && m_LastChatSend+time_freq() < time_get())
	{
		CHistoryEntry *pEntry = m_History.Last();
		for(int i = m_PendingChatCounter-1; pEntry; --i, pEntry = m_History.Prev(pEntry))
		{
			if(i == 0)
			{
				Say(pEntry->m_Team, pEntry->m_aText);
				break;
			}
		}
		--m_PendingChatCounter;
	}

	if (g_Config.m_HudModHideAll || g_Config.m_HudModHideChat)// || (!m_Show && m_Mode == MODE_NONE))
		return;
		
	CUIRect Screen = *UI()->Screen();

	static float ExtraMenus = 0.0f;
	static bool ExtraShown = false;
	
	Graphics()->MapScreen(Screen.x, Screen.y, Screen.x+Screen.w, Screen.y+Screen.h);
	Screen.HSplitBottom(400.0f-g_Config.m_ClChatHeightlimit/* -ExtraMenus */, 0, &Screen);
	Screen.VSplitLeft(300.0f*Graphics()->ScreenAspect(), &Screen, 0);
	Screen.Margin(10.0f, &Screen);
			
	float Width = Screen.w;
	float Height = Screen.h;
	
	CUIRect View = Screen;
	if(IsActive())
		m_MousePos = ControllerNui()->GetMousePosClamp(vec4(View.x, View.y-5, View.x+View.w, View.y+View.h));
	//m_MousePos.x = clamp(m_MousePos.x, View.x, View.x+View.w);
	//m_MousePos.y = clamp(m_MousePos.y, View.y-5, View.y+View.h);
	CUIRect Header, Footer, Down, TypeBox, Scroll, Row, d_Extra, d_Left, d_Right, d_Bottom, d_Top;
	
	View.HSplitTop(17.0f, &Header, &View);
	View.HSplitBottom(36.0f, &View, &TypeBox);
	View.HSplitBottom(17.0f+ExtraMenus, &View, &Footer);
		
	static float m_ShowFade = 0.0f;
	if(m_Show || m_Mode != MODE_NONE)
	{
		if(m_ShowFade < 1.0f)
			m_ShowFade += Client()->RenderFrameTime()*5.0f;
	}
	else
	{
		if(m_ShowFade > 0.0f)
			m_ShowFade -= Client()->RenderFrameTime()*5.0f;
	}
	
	static bool ChatMoving = false;
	if(m_ShowFade > 0.0f)
	{
		static int upBar = 0;
		static float upFade = 0;
		static int MousePosY = 0;
		static int ChatHeight = 0;
		
		int Inside = UI()->MouseInside(&Header);

		if(UI()->ActiveItem() == &upBar)
		{
			if(!UI()->MouseButton(0))
			{
				UI()->SetActiveItem(0);
				ChatMoving = true;			
			}
			g_Config.m_ClChatHeightlimit = ChatHeight + m_MousePos.y - MousePosY;
			g_Config.m_ClChatHeightlimit = clamp(g_Config.m_ClChatHeightlimit, 10, 248);
		}
		else if(UI()->HotItem() == &upBar)
		{
			if(UI()->MouseButton(0))
			{
				UI()->SetActiveItem(&upBar);
			}
			MousePosY = m_MousePos.y;
			ChatHeight = g_Config.m_ClChatHeightlimit;
		}

		if(Inside)
			UI()->SetHotItem(&upBar);	
		
		float *pUpperFade = ButtonFade(&upBar, &upFade, 0.6f);
		float UpperFadeVal = *pUpperFade/0.6f;
		
		vec4 UpperColor = mix(vec4(1.0f, 1.0f, 1.0f, 0.35f*m_ShowFade), vec4(0.5f, 0.5f, 0.5f, 0.55f*m_ShowFade), UpperFadeVal);

		RenderTools()->DrawUIRect(&Header, UpperColor, CUI::CORNER_T, 5.0f);
		UI()->DoLabel(&Header, "Chat", Header.h*0.8f, 0);

		// draw footers
		RenderTools()->DrawUIRect(&TypeBox, vec4(0,0,0,0.35f*m_ShowFade), CUI::CORNER_B, 5.0f);
		
		//RenderTools()->DrawUIRect(&Footer, vec4(1,1,1,0.35f*m_ShowFade), 0, 5.0f);
		
		
		//View.HSplitTop(17.0f, &Header, &View);
		//View.HSplitBottom(36.0f, &View, &TypeBox);
		//View.HSplitBottom(17.0f+ExtraMenus, &View, &Footer);
		
		Footer.HSplitTop(17.0f, &d_Top, &Footer);
				
		static int upExtra = 0;
		static float ExtraFade[3] = {0};
		static float ExtraHeight = 0.0f;
		int Inside2 = UI()->MouseInside(&d_Top);
		
		bool TempShown = false;
		if(UI()->ActiveItem() == &upExtra)
		{
			if(!UI()->MouseButton(0))
			{
				UI()->SetActiveItem(0);
			//	ChatMoving = true;			
			}
			ExtraMenus = ExtraHeight + MousePosY - m_MousePos.y;
			ExtraMenus = clamp(ExtraMenus, 0.0f, 84.0f);
			TempShown = true;
		}
		else if(UI()->HotItem() == &upExtra)
		{
			if(UI()->MouseButton(0))
			{
				UI()->SetActiveItem(&upExtra);
				//if(
			}
			MousePosY = m_MousePos.y;
			ExtraHeight = ExtraMenus;
		}
		
		if(!TempShown)
		{
			if(ExtraShown)
			{
				if(ExtraMenus <= 84.0f/4*3 && ExtraMenus > 0.0f)
					ExtraMenus-=2;
				else if(ExtraMenus > 84.0f/4*3 && ExtraMenus < 84.0f)
					ExtraMenus+=2;
			}
			else
			{
				if(ExtraMenus <= 84.0f/4 && ExtraMenus > 0.0f)
					ExtraMenus-=2;
				else if(ExtraMenus > 84.0f/4 && ExtraMenus < 84.0f)
					ExtraMenus+=2;
			}
		}
		ExtraMenus = clamp(ExtraMenus, 0.0f, 84.0f);
		if(ExtraMenus >= 84.0f)
			ExtraShown = true;
		else if(ExtraMenus <= 0.0f)
			ExtraShown = false;
			

		if(Inside2)
			UI()->SetHotItem(&upExtra);	
		
		float *pUpperFade2 = ButtonFade(&upExtra, &ExtraFade[0], 0.6f);
		float UpperFadeVal2 = *pUpperFade2/0.6f;
		
		vec4 UpperColor2 = mix(vec4(1.0f, 1.0f, 1.0f, 0.35f*m_ShowFade), vec4(0.5f, 0.5f, 0.5f, 0.55f*m_ShowFade), UpperFadeVal2);

		RenderTools()->DrawUIRect(&d_Top, UpperColor2, 0, 5.0f);
		
		if(ExtraMenus > 0.0f)
		{
		
		
		
			Footer.HSplitBottom(17.0f, &d_Extra, &d_Bottom);
			RenderTools()->DrawUIRect(&d_Bottom, vec4(1,1,1,0.35f*m_ShowFade), 0, 5.0f);
		
		
		
		
		
		
		}
/* 		Footer.VSplitLeft(10.0f, 0, &Footer);
		
		if(m_Mode == MODE_ALL)
			UI()->DoLabel(&Footer, Localize("All"), Footer.h*0.8f, 0);
		else if(m_Mode == MODE_TEAM)
			UI()->DoLabel(&Footer, Localize("Team"), Footer.h*0.8f, 0);
		else if(m_Mode != MODE_NONE)
			UI()->DoLabel(&Footer, Localize("Chat"), Footer.h*0.8f, 0); */

		// background
		RenderTools()->DrawUIRect(&View, vec4(0,0,0,0.25f*m_ShowFade), 0, 0);
	}
	else
	{
		ExtraMenus = 0.0f;
	}

	if(m_Mode != MODE_NONE)
	{
		// render chat input
		CTextCursor Cursor;
		TextRender()->SetCursor(&Cursor, TypeBox.x, TypeBox.y, 14.0f, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = TypeBox.w;
		Cursor.m_MaxLines = 2;

		// check if the visible text has to be moved
		if(m_InputUpdate)
		{
			if(m_ChatStringOffset > 0 && m_Input.GetLength() < m_OldChatStringLength)
				m_ChatStringOffset = max(0, m_ChatStringOffset-(m_OldChatStringLength-m_Input.GetLength()));

			if(m_ChatStringOffset > m_Input.GetCursorOffset())
				m_ChatStringOffset -= m_ChatStringOffset-m_Input.GetCursorOffset();
			else
			{
				CTextCursor Temp = Cursor;
				Temp.m_Flags = 0;
				TextRender()->TextEx(&Temp, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
				TextRender()->TextEx(&Temp, "|", -1);
				while(Temp.m_LineCount > 2)
				{
					++m_ChatStringOffset;
					Temp = Cursor;
					Temp.m_Flags = 0;
					TextRender()->TextEx(&Temp, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
					TextRender()->TextEx(&Temp, "|", -1);
				}
			}
			m_InputUpdate = false;
		}

		TextRender()->TextEx(&Cursor, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
		static float MarkerOffset = TextRender()->TextWidth(0, 8.0f, "|", -1)/3;
		CTextCursor Marker = Cursor;
		Marker.m_X -= MarkerOffset;
		if((2*time_get()/time_freq()) % 2)	// make it blink
			TextRender()->TextEx(&Marker, "|", -1);
		TextRender()->TextEx(&Cursor, m_Input.GetString()+m_Input.GetCursorOffset(), -1);
	}


	UI()->ClipEnable(&View);
	
	static float s_ScrollValue = 1.0f;
	int ScrollPos = (1.0f-s_ScrollValue)*View.y;
	int x = View.x;
	//int Temp
	int y = TypeBox.y - 17.0f; // View.y + View.h;// - ChatY;
	
	int TempY = y - 10.0f;
	
	int64 Now = time_get();
	float LineWidth = View.w - 15.0f;//m_pClient->m_pScoreboard->Active() ? 90.0f : 200.0f;
	float HeightLimit = View.y;//m_pClient->m_pScoreboard->Active() ? 230.0f : m_Show ? 50.0f : 200.0f;
	float Begin = x;
	float FontSize = 14.0f;
	CTextCursor Cursor;
	int OffsetType = m_pClient->m_pScoreboard->Active() ? 1 : 0;
	for(int i = 0; i < MAX_LINES; i++)
	{
		int r = ((m_CurrentLine-i)+MAX_LINES)%MAX_LINES;
		int r_prev = ((m_CurrentLine-i-1)+MAX_LINES)%MAX_LINES;

		// get the y offset (calculate it if we haven't done that yet)
		if(m_aLines[r].m_YNew[OffsetType] < 0.0f)
		{
			TextRender()->SetCursor(&Cursor, Begin, 0.0f, FontSize, 0);				
			if(m_aLines[r].m_Tee.m_Texture != -1)
				Cursor.m_X += 22.0f;
			Cursor.m_LineWidth = LineWidth;
			TextRender()->TextEx(&Cursor, m_aLines[r].m_aName, -1);
			TextRender()->TextEx(&Cursor, m_aLines[r].m_aText, -1);
			m_aLines[r].m_YOld[OffsetType] = y; 
			m_aLines[r].m_YNew[OffsetType] = Cursor.m_Y + Cursor.m_FontSize;
		}

		TempY -= m_aLines[r].m_YNew[OffsetType];
		
		if(m_aLines[r_prev].m_YNew[OffsetType] >= 0.0f && i != MAX_LINES - 1)
		{
			if(m_aLines[r].m_YOld[OffsetType] < m_aLines[r_prev].m_YOld[OffsetType] + m_aLines[r_prev].m_YNew[OffsetType])
				continue;
		}
		if(m_aLines[r].m_YOld[OffsetType] > TempY)
		{
			m_aLines[r].m_YOld[OffsetType] --;
		}
		else if(m_aLines[r].m_YOld[OffsetType] < TempY)
		{
			m_aLines[r].m_YOld[OffsetType] ++;
		}
		
		if(!m_Show && m_aLines[r].m_Time+g_Config.m_ClChatShowtime*time_freq() < Now)
		{
			if(m_aLines[r].m_Blend > 0.0f)
				m_aLines[r].m_Blend -= 1.0f*Client()->RenderFrameTime()*2.0f;
			if(m_aLines[r].m_Blend < 0.0f)
				m_aLines[r].m_Blend = 0.0f;
		}
		else
		{
			if(m_aLines[r].m_Blend < 1.0f)
				m_aLines[r].m_Blend += 1.0f*Client()->RenderFrameTime()*2.0f;
			if(m_aLines[r].m_Blend > 1.0f)
				m_aLines[r].m_Blend = 1.0f;
		}
		
		// cut off if msgs waste too much space
		if(m_aLines[r].m_YOld[OffsetType] + m_aLines[r].m_YNew[OffsetType] + 10.0f < HeightLimit)
			continue;
		
		if(ChatMoving && m_aLines[r_prev].m_YOld[OffsetType] + m_aLines[r_prev].m_YNew[OffsetType] + 10.0f < HeightLimit)
		{
			if((int)(Header.y + Header.h - y - 8.0f) % 14 != 0)
				g_Config.m_ClChatHeightlimit++;
			if((int)(Header.y + Header.h - y - 8.0f) % 14 == 0)
				ChatMoving = false;
		}
		
		float Blend = m_aLines[r].m_Blend;
		
		// reset the cursor
		TextRender()->SetCursor(&Cursor, Begin, m_aLines[r].m_YOld[OffsetType]+ScrollPos, FontSize, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = LineWidth;

		if(m_aLines[r].m_Tee.m_Texture != -1)
		{
			CTeeRenderInfo TeeInfo = m_aLines[r].m_Tee;
			TeeInfo.m_Size = 20.0f;
			TeeInfo.m_ColorBody.a = Blend;
			TeeInfo.m_ColorFeet.a = Blend;
			RenderTools()->RenderTee(CAnimState::GetIdle(), &TeeInfo, EMOTE_NORMAL, vec2(1.0f, 0.0f), vec2(Cursor.m_X+TeeInfo.m_Size/2, Cursor.m_Y+14.0f));
			Cursor.m_X += 22.0f;
		}

		// render name
		if(m_aLines[r].m_ClientID == -1)
			TextRender()->TextColor(1.0f, 1.0f, 0.5f, Blend); // system
		else if(m_aLines[r].m_Team)
			TextRender()->TextColor(0.45f, 0.9f, 0.45f, Blend); // team message
		else if(m_aLines[r].m_NameColor == TEAM_RED)
			TextRender()->TextColor(1.0f, 0.5f, 0.5f, Blend); // red
		else if(m_aLines[r].m_NameColor == TEAM_BLUE)
			TextRender()->TextColor(0.7f, 0.7f, 1.0f, Blend); // blue
		else if(m_aLines[r].m_NameColor == TEAM_SPECTATORS)
			TextRender()->TextColor(0.75f, 0.5f, 0.75f, Blend); // spectator
		else
			TextRender()->TextColor(0.8f, 0.8f, 0.8f, Blend);

		TextRender()->TextEx(&Cursor, m_aLines[r].m_aName, -1);

		// render line
		if(m_aLines[r].m_ClientID == -1)
			TextRender()->TextColor(1.0f, 1.0f, 0.5f, Blend); // system
		else if(m_aLines[r].m_Highlighted)
			TextRender()->TextColor(1.0f, 0.5f, 0.5f, Blend); // highlighted
		else if(m_aLines[r].m_Team)
			TextRender()->TextColor(0.65f, 1.0f, 0.65f, Blend); // team message
		else
			TextRender()->TextColor(1.0f, 1.0f, 1.0f, Blend);
		
		TextRender()->TextEx(&Cursor, m_aLines[r].m_aText, -1);		
	}
		
	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f); 
	
	UI()->ClipDisable();
	
	
	if(m_Show || m_Mode != MODE_NONE)
	{
		float RowHeight = 24.0f;
		// do the scrollbar
		static int s_ScrollBar = 0;
		static float s_FadeValue[2] = {0};

		int NumViewable = (int)(View.h/RowHeight) + 1;
		int Num = MAX_LINES-NumViewable+1;
		if(Num < 0)
			Num = 0;
		if(Num > 0)
		{
			if(Input()->KeyPresses(KEY_MOUSE_WHEEL_UP) && UI()->MouseInside(&View))
				s_ScrollValue -= 3.0f/Num;
			if(Input()->KeyPresses(KEY_MOUSE_WHEEL_DOWN) && UI()->MouseInside(&View))
				s_ScrollValue += 3.0f/Num;

			if(s_ScrollValue < 0.0f) s_ScrollValue = 0.0f;
			if(s_ScrollValue > 1.0f) s_ScrollValue = 1.0f;
		
			// prepare the scroll
			View.VSplitRight(15.0f, &View, &Scroll);

			
			//Scroll.HMargin(5.0f, &Scroll);
			int SizeSlider = View.h * (View.h / ((Num + NumViewable - 1) * RowHeight));	
			s_ScrollValue = DoScrollbarV(&s_ScrollBar, &s_FadeValue[0], &Scroll, s_ScrollValue, SizeSlider);
		}
		
		// update the ui
		int Buttons = 0;
		
		if(Input()->KeyPressed(KEY_MOUSE_1)) Buttons |= 1;
		if(Input()->KeyPressed(KEY_MOUSE_2)) Buttons |= 2;
		if(Input()->KeyPressed(KEY_MOUSE_3)) Buttons |= 4;
		
		UI()->Update(m_MousePos.x,m_MousePos.y,m_MousePos.x*3.0f,m_MousePos.y*3.0f,Buttons);

		// render cursor
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_CURSOR].m_Id);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(1,1,1,1);
		IGraphics::CQuadItem QuadItem(m_MousePos.x, m_MousePos.y, 24, 24);
		Graphics()->QuadsDrawTL(&QuadItem, 1);
		Graphics()->QuadsEnd();
	}	
}

void CChat::OnRender()
{
	OnRenderNew();
	return;
	
	// send pending chat messages
	if(m_PendingChatCounter > 0 && m_LastChatSend+time_freq() < time_get())
	{
		CHistoryEntry *pEntry = m_History.Last();
		for(int i = m_PendingChatCounter-1; pEntry; --i, pEntry = m_History.Prev(pEntry))
		{
			if(i == 0)
			{
				Say(pEntry->m_Team, pEntry->m_aText);
				break;
			}
		}
		--m_PendingChatCounter;
	}

	if (g_Config.m_HudModHideAll || g_Config.m_HudModHideChat)
		return;

	float Width = 300.0f*Graphics()->ScreenAspect();
	Graphics()->MapScreen(0.0f, 0.0f, Width, 300.0f);
	float x = 5.0f;
	float y = 300.0f-20.0f;
	if(m_Mode != MODE_NONE)
	{
		// render chat input
		CTextCursor Cursor;

		TextRender()->SetCursor(&Cursor, x, y, 8.0f, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = Width-190.0f;
		Cursor.m_MaxLines = 2;

		if(m_Mode == MODE_ALL)
			TextRender()->TextEx(&Cursor, Localize("All"), -1);
		else if(m_Mode == MODE_TEAM)
			TextRender()->TextEx(&Cursor, Localize("Team"), -1);
		else
			TextRender()->TextEx(&Cursor, Localize("Chat"), -1);

		TextRender()->TextEx(&Cursor, ": ", -1);

		// check if the visible text has to be moved
		if(m_InputUpdate)
		{
			if(m_ChatStringOffset > 0 && m_Input.GetLength() < m_OldChatStringLength)
				m_ChatStringOffset = max(0, m_ChatStringOffset-(m_OldChatStringLength-m_Input.GetLength()));

			if(m_ChatStringOffset > m_Input.GetCursorOffset())
				m_ChatStringOffset -= m_ChatStringOffset-m_Input.GetCursorOffset();
			else
			{
				CTextCursor Temp = Cursor;
				Temp.m_Flags = 0;
				TextRender()->TextEx(&Temp, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
				TextRender()->TextEx(&Temp, "|", -1);
				while(Temp.m_LineCount > 2)
				{
					++m_ChatStringOffset;
					Temp = Cursor;
					Temp.m_Flags = 0;
					TextRender()->TextEx(&Temp, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
					TextRender()->TextEx(&Temp, "|", -1);
				}
			}
			m_InputUpdate = false;
		}

		TextRender()->TextEx(&Cursor, m_Input.GetString()+m_ChatStringOffset, m_Input.GetCursorOffset()-m_ChatStringOffset);
		static float MarkerOffset = TextRender()->TextWidth(0, 8.0f, "|", -1)/3;
		CTextCursor Marker = Cursor;
		Marker.m_X -= MarkerOffset;
		TextRender()->TextEx(&Marker, "|", -1);
		TextRender()->TextEx(&Cursor, m_Input.GetString()+m_Input.GetCursorOffset(), -1);
	}

	y -= 8.0f;

	int64 Now = time_get();
	float LineWidth = m_pClient->m_pScoreboard->Active() ? 90.0f : 200.0f;
	float HeightLimit = m_pClient->m_pScoreboard->Active() ? 230.0f : m_Show ? 50.0f : 200.0f;
	float Begin = x;
	float FontSize = 6.0f;
	CTextCursor Cursor;
	int OffsetType = m_pClient->m_pScoreboard->Active() ? 1 : 0;
	for(int i = 0; i < MAX_LINES; i++)
	{
		int r = ((m_CurrentLine-i)+MAX_LINES)%MAX_LINES;
		if(Now > m_aLines[r].m_Time+16*time_freq() && !m_Show)
			break;

		// get the y offset (calculate it if we haven't done that yet)
		if(m_aLines[r].m_YNew[OffsetType] < 0.0f)
		{
			TextRender()->SetCursor(&Cursor, Begin, 0.0f, FontSize, 0);
			Cursor.m_LineWidth = LineWidth;
			TextRender()->TextEx(&Cursor, m_aLines[r].m_aName, -1);
			TextRender()->TextEx(&Cursor, m_aLines[r].m_aText, -1);
			m_aLines[r].m_YNew[OffsetType] = Cursor.m_Y + Cursor.m_FontSize;
		}
		y -= m_aLines[r].m_YNew[OffsetType];

		// cut off if msgs waste too much space
		if(y < HeightLimit)
			break;

		float Blend = Now > m_aLines[r].m_Time+14*time_freq() && !m_Show ? 1.0f-(Now-m_aLines[r].m_Time-14*time_freq())/(2.0f*time_freq()) : 1.0f;

		// reset the cursor
		TextRender()->SetCursor(&Cursor, Begin, y, FontSize, TEXTFLAG_RENDER);
		Cursor.m_LineWidth = LineWidth;

		// render name
		if(m_aLines[r].m_ClientID == -1)
			TextRender()->TextColor(1.0f, 1.0f, 0.5f, Blend); // system
		else if(m_aLines[r].m_Team)
			TextRender()->TextColor(0.45f, 0.9f, 0.45f, Blend); // team message
		else if(m_aLines[r].m_NameColor == TEAM_RED)
			TextRender()->TextColor(1.0f, 0.5f, 0.5f, Blend); // red
		else if(m_aLines[r].m_NameColor == TEAM_BLUE)
			TextRender()->TextColor(0.7f, 0.7f, 1.0f, Blend); // blue
		else if(m_aLines[r].m_NameColor == TEAM_SPECTATORS)
			TextRender()->TextColor(0.75f, 0.5f, 0.75f, Blend); // spectator
		else
			TextRender()->TextColor(0.8f, 0.8f, 0.8f, Blend);

		TextRender()->TextEx(&Cursor, m_aLines[r].m_aName, -1);

		// render line
		if(m_aLines[r].m_ClientID == -1)
			TextRender()->TextColor(1.0f, 1.0f, 0.5f, Blend); // system
		else if(m_aLines[r].m_Highlighted)
			TextRender()->TextColor(1.0f, 0.5f, 0.5f, Blend); // highlighted
		else if(m_aLines[r].m_Team)
			TextRender()->TextColor(0.65f, 1.0f, 0.65f, Blend); // team message
		else
			TextRender()->TextColor(1.0f, 1.0f, 1.0f, Blend);

		TextRender()->TextEx(&Cursor, m_aLines[r].m_aText, -1);
	}

	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CChat::Say(int Team, const char *pLine)
{
	m_LastChatSend = time_get();

	// send chat message
	CNetMsg_Cl_Say Msg;
	Msg.m_Team = Team;
	Msg.m_pMessage = pLine;
	Client()->SendPackMsg(&Msg, MSGFLAG_VITAL);
}
