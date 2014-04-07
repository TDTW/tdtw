/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/demo.h>
#include <engine/graphics.h>
#include <engine/textrender.h>
#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include <game/client/ui.h>
#include <game/client/render.h>
#include "test.h"

CTest::CTest()
{
	m_NumItems = 0;
}

CTest::CItem *CTest::CreateI()
{
	if (m_NumItems < MAX_ITEMS)
	{
		CItem *p = &m_aItems[m_NumItems];
		m_NumItems++;
		return p;
	}
	return 0;
}

void CTest::DestroyI(CTest::CItem *i)
{
	m_NumItems--;
	*i = m_aItems[m_NumItems];
}

bool CTest::SearchType(int Type)
{
	for(int i=0; i<m_NumItems; i++)
	{
		if(m_aItems[i].m_Type == Type)
			return true;
	}
	return false;
}

void CTest::Create(vec2 Pos, int Type)
{	
	if(g_Config.m_HudModHideBdadash || g_Config.m_HudModHideAll)
		return;
	CItem *i = CreateI();
	if (i)
	{
		i->m_StartPos = vec2(Pos.x, Pos.y - 50);
		i->m_EndPos = vec2(Pos.x + rand()%80-40, Pos.y - 80);
		i->m_StartTime = Client()->LocalTime();
		i->m_Speed = S_FAST;
		i->m_Size = Z_SMAL;
		
		if(str_find(g_Config.m_ClLanguagefile, "russian") || 
			str_find(g_Config.m_ClLanguagefile, "ukrainian") ||
			str_find(g_Config.m_ClLanguagefile, "belarusian"))
		{
			switch(Type)
			{
			case SOUND_GUN_FIRE: // тра - та та та та 
				if(SearchType(SOUND_GUN_FIRE))
				{
					if(rand()%2==0)
						str_copy(i->m_Text, "Та", sizeof(i->m_Text)); //Та
					else
						str_copy(i->m_Text, "Да", sizeof(i->m_Text));	//Да			
				}
				else
				{			
					str_copy(i->m_Text, "Тра", sizeof(i->m_Text)); //Тра
					i->m_Size = Z_LARG;
				}
				break;
			case SOUND_SHOTGUN_FIRE: // пух - пыщ
				if(rand()%2==0)
					str_copy(i->m_Text, "Пух", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Пыщ", sizeof(i->m_Text));
				break;
			case SOUND_GRENADE_FIRE: // Бум
				str_copy(i->m_Text, "Бум", sizeof(i->m_Text));
				break;
			case SOUND_GRENADE_EXPLODE: // бдыщ
				str_copy(i->m_Text, "БДЫЩ", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_HAMMER_FIRE: // фух
				str_copy(i->m_Text, "Фух", sizeof(i->m_Text));
				break;
			case SOUND_HAMMER_HIT: // дыщ
				str_copy(i->m_Text, "Дыщ", sizeof(i->m_Text));
				i->m_Size = Z_MID;
				break;
			case SOUND_NINJA_FIRE: // кьяяя
				str_copy(i->m_Text, "КЬЯЯ", sizeof(i->m_Text));
				i->m_Size = Z_MID;
				break;
			case SOUND_PICKUP_NINJA: // кьяяя
				str_copy(i->m_Text, "КЬЯЯ", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_NINJA_HIT: // хряк
				str_copy(i->m_Text, "Хряк", sizeof(i->m_Text));
				i->m_Size = Z_LARG;
				break;
			case SOUND_RIFLE_FIRE: // дзз
				str_copy(i->m_Text, "Пиу", sizeof(i->m_Text));
				break;
			case SOUND_RIFLE_BOUNCE: // дзз
				str_copy(i->m_Text, "Дзз", sizeof(i->m_Text));
				break;
			case SOUND_WEAPON_SWITCH: // фух
				str_copy(i->m_Text, "Фух", sizeof(i->m_Text));
				break;
			case SOUND_PLAYER_PAIN_SHORT: // Ааа
				if(rand()%3==0)
					str_copy(i->m_Text, "Ааа", sizeof(i->m_Text));
				else if(rand()%3==1)
					str_copy(i->m_Text, "Аай", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Оой", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_PLAYER_PAIN_LONG: // АаАаА
				str_copy(i->m_Text, "АаАаа", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_DIE: // Труп
				if(rand()%3==0)
					str_copy(i->m_Text, "Труп", sizeof(i->m_Text));
				else if(rand()%3==1)
					str_copy(i->m_Text, "Здох", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Умер", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_SPAWN: // Вжух
				str_copy(i->m_Text, "Вжух", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_SKID: // УИИИ
				str_copy(i->m_Text, "УИИии", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;

			case SOUND_PICKUP_HEALTH: // Уоп
				str_copy(i->m_Text, "Уоп", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_WEAPON_NOAMMO: // Чик
				str_copy(i->m_Text, "Чик", sizeof(i->m_Text));
				break;
			case SOUND_PICKUP_ARMOR: // Чик
				str_copy(i->m_Text, "Чик", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_PICKUP_GRENADE: // Чик
			case SOUND_PICKUP_SHOTGUN: // Чик
				str_copy(i->m_Text, "Пик", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_WEAPON_SPAWN: // Уоп
				str_copy(i->m_Text, "Уоп", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_MID;
				break;
			default:
				DestroyI(i);
				return;
			}
		}
		else
		{
			switch(Type)
			{
			case SOUND_GUN_FIRE: // тра - та та та та 
				if(SearchType(SOUND_GUN_FIRE))
				{
					str_copy(i->m_Text, "Ta", sizeof(i->m_Text));				
				}
				else
				{			
					str_copy(i->m_Text, "Ra", sizeof(i->m_Text)); 
					i->m_Size = Z_LARG;
				}
				break;
			case SOUND_SHOTGUN_FIRE: // пух - пыщ
				if(rand()%2==0)
					str_copy(i->m_Text, "Bang", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Blam", sizeof(i->m_Text));
				break;
			case SOUND_GRENADE_FIRE: // Бум
				str_copy(i->m_Text, "Poom", sizeof(i->m_Text));
				break;
			case SOUND_GRENADE_EXPLODE: // бдыщ
				str_copy(i->m_Text, "BOOM", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_HAMMER_FIRE: // фух
				str_copy(i->m_Text, "Hop", sizeof(i->m_Text));
				break;
			case SOUND_HAMMER_HIT: // дыщ
				str_copy(i->m_Text, "Hit", sizeof(i->m_Text));
				i->m_Size = Z_MID;
				break;
			case SOUND_NINJA_FIRE: // кьяяя
				str_copy(i->m_Text, "KYaaaw", sizeof(i->m_Text));
				i->m_Size = Z_MID;
				break;
			case SOUND_PICKUP_NINJA: // кьяяя
				str_copy(i->m_Text, "KYaaaw", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_NINJA_HIT: // хряк
				str_copy(i->m_Text, "SLash", sizeof(i->m_Text));
				i->m_Size = Z_LARG;
				break;
			case SOUND_RIFLE_FIRE: // дзз
				str_copy(i->m_Text, "Phew", sizeof(i->m_Text));
				break;
			case SOUND_RIFLE_BOUNCE: // дзз
				str_copy(i->m_Text, "Zap", sizeof(i->m_Text));
				break;
			case SOUND_WEAPON_SWITCH: // фух
				str_copy(i->m_Text, "Wosh", sizeof(i->m_Text));
				break;
			case SOUND_PLAYER_PAIN_SHORT: // Ааа
				if(rand()%2==0)
					str_copy(i->m_Text, "Aaa", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Ouch", sizeof(i->m_Text));			
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_PLAYER_PAIN_LONG: // АаАаА
				str_copy(i->m_Text, "AaAaa", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_DIE: // Труп
				if(rand()%2==0)
					str_copy(i->m_Text, "Died", sizeof(i->m_Text));
				else
					str_copy(i->m_Text, "Crack", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_SPAWN: // Вжух
				str_copy(i->m_Text, "Yey!", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_LARG;
				break;
			case SOUND_PLAYER_SKID: // УИИИ
				str_copy(i->m_Text, "Shh", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_PICKUP_HEALTH: // Уоп
				str_copy(i->m_Text, "Whop", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_WEAPON_NOAMMO: // Чик
				str_copy(i->m_Text, "Click", sizeof(i->m_Text));
				break;
			case SOUND_PICKUP_ARMOR: // Чик
				str_copy(i->m_Text, "Trick", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_PICKUP_GRENADE: // Чик
			case SOUND_PICKUP_SHOTGUN: // Чик
				str_copy(i->m_Text, "Pick", sizeof(i->m_Text));
				i->m_Speed = S_MID;
				i->m_Size = Z_MID;
				break;
			case SOUND_WEAPON_SPAWN: // Уоп
				str_copy(i->m_Text, "Whop", sizeof(i->m_Text));
				i->m_Speed = S_SLOW;
				
				i->m_Size = Z_MID;
				break;
			default:
				DestroyI(i);
				return;
			}
		}
		
		i->m_Type = Type;
	}
}

void CTest::OnRender()
{
	static float s_LastLocalTime = Client()->LocalTime();
	for(int i = 0; i < m_NumItems;)
	{
		//if(i >= MAX_ITEMS)
		//	break;
		if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		{
			const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
			if(pInfo->m_Paused)
				m_aItems[i].m_StartTime += Client()->LocalTime()-s_LastLocalTime;
			else
				m_aItems[i].m_StartTime += (Client()->LocalTime()-s_LastLocalTime)*(1.0f-pInfo->m_Speed);
		}
		else
		{
			if(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_PAUSED)
				m_aItems[i].m_StartTime += Client()->LocalTime()-s_LastLocalTime;
		}

		float Speed;
		switch (m_aItems[i].m_Speed)
		{
		case S_SLOW:
			Speed = 0.75f;
			break;
		case S_MID:
			Speed = 0.50f;
			break;
		case S_FAST:
			Speed = 0.25f;
			break;
		default:
			Speed = 0.50f;
			break;			
		}
		
		float Size;
		switch (m_aItems[i].m_Size)
		{
		case Z_SMAL:
			Size = 18.0f;
			break;
		case Z_MID:
			Size = 22.0f;
			break;
		case Z_LARG:
			Size = 26.0f;
			break;
		default:
			Size = 22.0f;
			break;			
		}
		
		float Life = Speed - (Client()->LocalTime() - m_aItems[i].m_StartTime);
		if(Life < 0.0f)
			DestroyI(&m_aItems[i]);
		else
		{
			TextRender()->TextOutlineColor(0.0f, 0.0f, 0.0f, 0.5f*Life/0.1f);
			TextRender()->TextColor(1.0f, 1.0f, 1.0f, Life/0.1f);
			TextRender()->Text(0, m_aItems[i].m_StartPos.x, m_aItems[i].m_StartPos.y, Size, m_aItems[i].m_Text, -1);
			TextRender()->TextColor(1,1,1,1);
			TextRender()->TextOutlineColor(0.0f, 0.0f, 0.0f, 0.3f);
			if(m_aItems[i].m_StartPos.y > m_aItems[i].m_EndPos.y)
				m_aItems[i].m_StartPos.y--;
			if(m_aItems[i].m_StartPos.x > m_aItems[i].m_EndPos.x)
				m_aItems[i].m_StartPos.x--;
			if(m_aItems[i].m_StartPos.x < m_aItems[i].m_EndPos.x)
				m_aItems[i].m_StartPos.x++;
			i++;
		}
	}	
	s_LastLocalTime = Client()->LocalTime();
}



