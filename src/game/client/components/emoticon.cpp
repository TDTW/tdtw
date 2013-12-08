/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include <game/client/ui.h>
#include <game/client/render.h>
#include <game/client/components/sounds.h>
#include "emoticon.h"

CEmoticon::CEmoticon()
{
	OnReset();
}

void CEmoticon::ConKeyEmoticon(IConsole::IResult *pResult, void *pUserData)
{
	CEmoticon *pSelf = (CEmoticon *)pUserData;
	if(!pSelf->m_pClient->m_Snap.m_SpecInfo.m_Active && pSelf->Client()->State() != IClient::STATE_DEMOPLAYBACK)
		pSelf->m_Active = pResult->GetInteger(0) != 0;
}

void CEmoticon::ConEmote(IConsole::IResult *pResult, void *pUserData)
{
	((CEmoticon *)pUserData)->Emote(pResult->GetInteger(0));
}

void CEmoticon::OnConsoleInit()
{
	Console()->Register("+emote", "", CFGFLAG_CLIENT, ConKeyEmoticon, this, "Open emote selector");
	Console()->Register("emote", "i", CFGFLAG_CLIENT, ConEmote, this, "Use emote");
}

void CEmoticon::OnReset()
{
	m_WasActive = false;
	m_Active = false;
	m_SelectedEmote = -1;
	m_OldEmote = -1;
	m_CircleSize = 50.0f;
	m_WhiteCircleSize = 25.0f;
	m_WhiteCircleCoord = vec2(0.0f, 0.0f);
	m_aLastSoundPlayed = 0;
	for(int i=0; i < NUM_EMOTICONS; i++)
		m_Size[i] = 0.0f;
}

void CEmoticon::OnRelease()
{
	m_Active = false;
}

void CEmoticon::OnMessage(int MsgType, void *pRawMsg)
{
}

bool CEmoticon::OnMouseMove(float x, float y)
{
	if(!m_Active)
		return false;

	UI()->ConvertMouseMove(&x, &y);
	m_SelectorMouse += vec2(x,y);
	return true;
}

void CEmoticon::DrawCircle(float x, float y, float start_r, float end_r, int Segments)
{
	IGraphics::CFreeformItem Array[32];
	int NumItems = 0;
	float FSegments = (float)Segments;
	for(int i = 0; i < Segments; i++)
	{
		float a1 = i/FSegments * 2*pi;
		float a2 = (i+1)/FSegments * 2*pi;
		float Ca1 = cosf(a1);
		float Ca2 = cosf(a2);
		float Sa1 = sinf(a1);
		float Sa2 = sinf(a2);

		Array[NumItems++] = IGraphics::CFreeformItem(
			x+Ca1*start_r, y+Sa1*start_r,
			x+Ca2*start_r, y+Sa2*start_r,
			x+Ca1*end_r, y+Sa1*end_r,
			x+Ca2*end_r, y+Sa2*end_r);
		if(NumItems == 32)
		{
			m_pClient->Graphics()->QuadsDrawFreeform(Array, 32);
			NumItems = 0;
		}
	}
	if(NumItems)
		m_pClient->Graphics()->QuadsDrawFreeform(Array, NumItems);
}


void CEmoticon::OnRender()
{
	if(!m_Active && m_CircleSize <= 50.0f)
	{
		if(m_WasActive && m_SelectedEmote != -1)
			Emote(m_SelectedEmote);
		m_WasActive = false;
		return;
	}

	if(m_pClient->m_Snap.m_SpecInfo.m_Active)
	{
		m_Active = false;
		m_WasActive = false;
		return;
	}

	m_WasActive = true;

	CUIRect Screen = *UI()->Screen();

	Graphics()->MapScreen(Screen.x, Screen.y, Screen.w, Screen.h);

	Graphics()->BlendNormal();

	static bool AllClear = false;
	
	// Circle
	if(m_CircleSize < 190.0f && m_Active)
		m_CircleSize += (190.0f-50.0f)*Client()->RenderFrameTime()*4.0f;
	else if(!m_Active && m_CircleSize > 50.0f && AllClear)
		m_CircleSize -= (190.0f-50.0f)*Client()->RenderFrameTime()*4.0f;		
	
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(0,0,0,(((m_CircleSize-50.0f)/140.0f)*0.3f));
	DrawCircle(Screen.w/2, Screen.h/2, m_CircleSize*0.55f, m_CircleSize, 64);
	Graphics()->QuadsEnd();
	
	// Mouse cliping
	if (length(m_SelectorMouse) > 170.0f)
		m_SelectorMouse = normalize(m_SelectorMouse) * 170.0f;

	// Selected emote
	float SelectedAngle = GetAngle(m_SelectorMouse) + 2*pi/24;
	if (SelectedAngle < 0)
		SelectedAngle += 2*pi;

	if (length(m_SelectorMouse) > 110.0f)
		m_SelectedEmote = (int)(SelectedAngle / (2*pi) * NUM_EMOTICONS);
	
	if(m_OldEmote != m_SelectedEmote)
	{
		if(time_get()-m_aLastSoundPlayed >= time_freq()*4/10)
		{
			m_pClient->m_pSounds->Play(CSounds::CHN_GUI, SOUND_HOOK_LOOP, 0);
			m_aLastSoundPlayed = time_get();
		}
			
		m_OldEmote = m_SelectedEmote;
	}
		
	if (length(m_SelectorMouse) < 110.0f || !m_Active)
	{
		if(m_WhiteCircleSize < m_CircleSize*0.45f)
		{
			m_WhiteCircleSize += (m_CircleSize*0.45f)*Client()->RenderFrameTime()*4.0f;
			if(m_WhiteCircleSize >= m_CircleSize*0.45f)
				m_WhiteCircleSize = m_CircleSize*0.45f;
		}
		else
		{
			m_WhiteCircleSize -= (m_CircleSize*0.45f)*Client()->RenderFrameTime()*4.0f;
			if(m_WhiteCircleSize <= m_CircleSize*0.45f)
				m_WhiteCircleSize = m_CircleSize*0.45f;
		}
			
		if(m_WhiteCircleCoord == vec2(0.0f, 0.0f))
		{
			m_WhiteCircleCoord = vec2(Screen.w/2, Screen.h/2);
			m_OldCoord = vec2(Screen.w/2, Screen.h/2);
		}
		else if(m_WhiteCircleCoord != vec2(Screen.w/2, Screen.h/2))
		{
			if(m_WhiteCircleCoord.x > Screen.w/2)
			{
				m_WhiteCircleCoord.x -= abs(m_OldCoord.x - Screen.w/2) * Client()->RenderFrameTime() * 4.0f;
				if(m_WhiteCircleCoord.x < Screen.w/2)
				{
					m_WhiteCircleCoord.x = Screen.w/2;
					m_OldCoord.x = Screen.w/2;
				}
			}
			else if(m_WhiteCircleCoord.x < Screen.w/2)
			{
				m_WhiteCircleCoord.x += abs(Screen.w/2 - m_OldCoord.x) * Client()->RenderFrameTime() * 4.0f;
				if(m_WhiteCircleCoord.x > Screen.w/2)
				{
					m_WhiteCircleCoord.x = Screen.w/2;
					m_OldCoord.x = Screen.w/2;
				}
			}
			
			if(m_WhiteCircleCoord.y > Screen.h/2)
			{
				m_WhiteCircleCoord.y -= abs(m_OldCoord.y - Screen.h/2) * Client()->RenderFrameTime() * 4.0f;
				if(m_WhiteCircleCoord.y < Screen.h/2)
				{
					m_WhiteCircleCoord.y = Screen.h/2;
					m_OldCoord.y = Screen.h/2;
				}
			}
			else if(m_WhiteCircleCoord.y < Screen.h/2)
			{
				m_WhiteCircleCoord.y += abs(Screen.h/2 - m_OldCoord.y) * Client()->RenderFrameTime() * 4.0f;
				if(m_WhiteCircleCoord.y > Screen.h/2)
				{
					m_WhiteCircleCoord.y = Screen.h/2;
					m_OldCoord.y = Screen.h/2;
				}
			}
		}
		if(m_Active)
		{
			m_SelectedEmote = -1;
			m_OldEmote = -1;
		}
	}
	else
	{
		if(m_WhiteCircleSize > 40.0f)
		{
			m_WhiteCircleSize -= (m_CircleSize*0.45f-40.0f)*Client()->RenderFrameTime()*4.0f;
			if(m_WhiteCircleSize <= 40.0f)
				m_WhiteCircleSize = 40.0f;
		}
		else
		{
			m_WhiteCircleSize += (m_CircleSize*0.45f-40.0f)*Client()->RenderFrameTime()*4.0f;
			if(m_WhiteCircleSize >= 40.0f)
				m_WhiteCircleSize = 40.0f;
		}
		
		float Angle = 2*pi*m_SelectedEmote/NUM_EMOTICONS;
		if (Angle > pi)
			Angle -= 2*pi;
			
		float NuX = Screen.w/2 + 150.0f * cosf(Angle);
		float NuY = Screen.h/2 + 150.0f * sinf(Angle);
		
		if(m_WhiteCircleCoord.x > NuX)
		{
			m_WhiteCircleCoord.x -= abs(m_OldCoord.x - NuX) * Client()->RenderFrameTime() * 4.0f;
			if(m_WhiteCircleCoord.x < NuX)
			{
				m_WhiteCircleCoord.x = NuX;
				m_OldCoord.x = NuX;
			}
		}
		else if(m_WhiteCircleCoord.x < NuX)
		{
			m_WhiteCircleCoord.x += abs(NuX - m_OldCoord.x) * Client()->RenderFrameTime() * 4.0f;
			if(m_WhiteCircleCoord.x > NuX)
			{
				m_WhiteCircleCoord.x = NuX;
				m_OldCoord.x = NuX;
			}
		}
		
		if(m_WhiteCircleCoord.y > NuY)
		{
			m_WhiteCircleCoord.y -= abs(m_OldCoord.y - NuY) * Client()->RenderFrameTime() * 4.0f;
			if(m_WhiteCircleCoord.y < NuY)
			{
				m_WhiteCircleCoord.y = NuY;
				m_OldCoord.y = NuY;
			}
		}
		else if(m_WhiteCircleCoord.y < NuY)
		{
			m_WhiteCircleCoord.y += abs(NuY - m_OldCoord.y) * Client()->RenderFrameTime() * 4.0f;
			if(m_WhiteCircleCoord.y > NuY)
			{
				m_WhiteCircleCoord.y = NuY;
				m_OldCoord.y = NuY;
			}
		}		
	}

	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1,1,1,(((m_CircleSize-50.0f)/140.0f)*0.3f));
	DrawCircle(m_WhiteCircleCoord.x, m_WhiteCircleCoord.y, 0.0f, m_WhiteCircleSize, 32);
	Graphics()->QuadsEnd();
	
	if(m_CircleSize < 190.0f)
		return;
		
	// Emoticons
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_EMOTICONS].m_Id);
	Graphics()->QuadsBegin();

	for (int i = 0; i < NUM_EMOTICONS; i++)
	{
		float Angle = 2*pi*i/NUM_EMOTICONS;
		if (Angle > pi)
			Angle -= 2*pi;

		bool Selected = m_SelectedEmote == i;

		float Size = Selected ? 80.0f : 50.0f;

		if(!m_Active)
		{
			if(m_Size[i] > 0.0f)
			{
				m_Size[i] -= 80.0f*Client()->RenderFrameTime()*4.0f;
				if(m_Size[i] <= 0.0f)
					m_Size[i] = 0.0f;
			}
			else
				AllClear = true;
		}
		else if(m_Size[i] < Size)
		{
			m_Size[i] += 80.0f*Client()->RenderFrameTime()*4.0f;
			if(m_Size[i] >= Size)
				m_Size[i] = Size;
			AllClear = false;
		}
		else if(m_Size[i] > Size)
		{
			m_Size[i] -= 80.0f*Client()->RenderFrameTime()*4.0f;
			if(m_Size[i] <= Size)
				m_Size[i] = Size;
		}
			
		float NudgeX = 150.0f * cosf(Angle);
		float NudgeY = 150.0f * sinf(Angle);
		RenderTools()->SelectSprite(SPRITE_OOP + i);
		IGraphics::CQuadItem QuadItem(Screen.w/2 + NudgeX, Screen.h/2 + NudgeY, m_Size[i], m_Size[i]);
		Graphics()->QuadsDraw(&QuadItem, 1);
	}

	Graphics()->QuadsEnd();

	// Cursor
	/*Graphics()->TextureSet(g_pData->m_aImages[IMAGE_CURSOR].m_Id);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1,1,1,1);
	IGraphics::CQuadItem QuadItem(m_SelectorMouse.x+Screen.w/2,m_SelectorMouse.y+Screen.h/2,24,24);
	Graphics()->QuadsDrawTL(&QuadItem, 1);
	Graphics()->QuadsEnd();*/
}

void CEmoticon::Emote(int Emoticon)
{
	CNetMsg_Cl_Emoticon Msg;
	Msg.m_Emoticon = Emoticon;
	Client()->SendPackMsg(&Msg, MSGFLAG_VITAL);
}
