/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/graphics.h>
#include <engine/demo.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/client/render.h>
#include <engine/shared/config.h>

#include <game/client/components/flow.h>
#include <game/client/components/effects.h>

#include "items.h"
#include "skins.h"

void CItems::OnReset()
{
	m_NumExtraProjectiles = 0;
}

void CItems::RenderProjectile(const CNetObj_Projectile *pCurrent, int ItemID)
{
	// get positions
	float Curvature = 0;
	float Speed = 0;
	if(pCurrent->m_Type == WEAPON_GRENADE)
	{
		Curvature = m_pClient->m_Tuning.m_GrenadeCurvature;
		Speed = m_pClient->m_Tuning.m_GrenadeSpeed;
	}
	else if(pCurrent->m_Type == WEAPON_SHOTGUN)
	{
		Curvature = m_pClient->m_Tuning.m_ShotgunCurvature;
		Speed = m_pClient->m_Tuning.m_ShotgunSpeed;
	}
	else if(pCurrent->m_Type == WEAPON_GUN)
	{
		Curvature = m_pClient->m_Tuning.m_GunCurvature;
		Speed = m_pClient->m_Tuning.m_GunSpeed;
	}

	static float s_LastGameTickTime = Client()->GameTickTime();
	if(m_pClient->m_Snap.m_pGameInfoObj && !(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_PAUSED))
		s_LastGameTickTime = Client()->GameTickTime();
	float Ct = (Client()->PrevGameTick()-pCurrent->m_StartTick)/(float)SERVER_TICK_SPEED + s_LastGameTickTime;
	if(Ct < 0)
		return; // projectile havn't been shot yet

	vec2 StartPos(pCurrent->m_X, pCurrent->m_Y);
	vec2 StartVel(pCurrent->m_VelX/100.0f, pCurrent->m_VelY/100.0f);
	vec2 Pos = CalcPos(StartPos, StartVel, Curvature, Speed, Ct);
	vec2 PrevPos = CalcPos(StartPos, StartVel, Curvature, Speed, Ct-0.001f);


	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
	Graphics()->QuadsBegin();

	RenderTools()->SelectSprite(g_pData->m_Weapons.m_aId[clamp(pCurrent->m_Type, 0, NUM_WEAPONS-1)].m_pSpriteProj);
	vec2 Vel = Pos-PrevPos;
	//vec2 pos = mix(vec2(prev->x, prev->y), vec2(current->x, current->y), Client()->IntraGameTick());


	// add particle for this projectile
	if(pCurrent->m_Type == WEAPON_GRENADE)
	{
		m_pClient->m_pEffects->SmokeTrail(Pos, Vel*-1);
		static float s_Time = 0.0f;
		static float s_LastLocalTime = Client()->LocalTime();

		if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		{
			const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
			if(!pInfo->m_Paused)
				s_Time += (Client()->LocalTime()-s_LastLocalTime)*pInfo->m_Speed;
		}
		else
		{
			if(m_pClient->m_Snap.m_pGameInfoObj && !(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_PAUSED))
				s_Time += Client()->LocalTime()-s_LastLocalTime;
		}

		Graphics()->QuadsSetRotation(s_Time*pi*2*2 + ItemID);
		s_LastLocalTime = Client()->LocalTime();
	}
	else
	{
		m_pClient->m_pEffects->BulletTrail(Pos);

		if(length(Vel) > 0.00001f)
			Graphics()->QuadsSetRotation(GetAngle(Vel));
		else
			Graphics()->QuadsSetRotation(0);

	}

	IGraphics::CQuadItem QuadItem(Pos.x, Pos.y, 32, 32);
	Graphics()->QuadsDraw(&QuadItem, 1);
	
	// Draw shadows of grenades
	bool LocalPlayerInGame = 
		m_pClient->m_aClients[m_pClient->m_Snap.m_pLocalInfo->m_ClientID].m_Team != -1;

	if(g_Config.m_AntiPing && pCurrent->m_Type == WEAPON_GRENADE && LocalPlayerInGame && !m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER)
	{
		// Calculate average prediction offset, because client_predtick() gets varial values :(((
		// Must be there is a normal way to realize it, but I'm too lazy to find it. ^)
		if (m_pClient->m_Average_Prediction_Offset == -1)
		{
			int Offset = Client()->PredGameTick() - Client()->GameTick();
			m_pClient->m_Prediction_Offset_Summ += Offset;
			m_pClient->m_Prediction_Offset_Count++;

			if (m_pClient->m_Prediction_Offset_Count >= 100)
			{
				m_pClient->m_Average_Prediction_Offset = 
					round((float)m_pClient->m_Prediction_Offset_Summ / m_pClient->m_Prediction_Offset_Count);
			}
		}		

		// Draw shadow only if grenade directed to local player
		CNetObj_CharacterCore& CurChar = m_pClient->m_Snap.m_aCharacters[m_pClient->m_Snap.m_pLocalInfo->m_ClientID].m_Cur;
		CNetObj_CharacterCore& PrevChar = m_pClient->m_Snap.m_aCharacters[m_pClient->m_Snap.m_pLocalInfo->m_ClientID].m_Prev;
		vec2 ServerPos = mix(vec2(PrevChar.m_X, PrevChar.m_Y), vec2(CurChar.m_X, CurChar.m_Y), Client()->IntraGameTick());

		float d1 = distance(Pos, ServerPos);
		float d2 = distance(PrevPos, ServerPos);
		if (d1 < 0) d1 *= -1;
		if (d2 < 0) d2 *= -1;
		bool GrenadeIsDirectedToLocalPlayer = d1 < d2;

		if (m_pClient->m_Average_Prediction_Offset != -1 && GrenadeIsDirectedToLocalPlayer)
		{
			int PredictedTick = Client()->PrevGameTick() + m_pClient->m_Average_Prediction_Offset;
			float PredictedCt = (PredictedTick - pCurrent->m_StartTick)/(float)SERVER_TICK_SPEED + Client()->GameTickTime();
		
			if (PredictedCt >= 0)
			{
				int shadow_type = WEAPON_GUN; // Pistol bullet sprite is used for marker of shadow. TODO: use something custom.
				RenderTools()->SelectSprite(g_pData->m_Weapons.m_aId[clamp(shadow_type, 0, NUM_WEAPONS-1)].m_pSpriteProj);
				
				vec2 PredictedPos = CalcPos(StartPos, StartVel, Curvature, Speed, PredictedCt);
				
				IGraphics::CQuadItem QuadItem(PredictedPos.x, PredictedPos.y, 32, 32);
				Graphics()->QuadsDraw(&QuadItem, 1);
			}
		}
	}
	
	Graphics()->QuadsSetRotation(0);
	Graphics()->QuadsEnd();
}

void CItems::RenderPickup(const CNetObj_Pickup *pPrev, const CNetObj_Pickup *pCurrent)
{
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
	Graphics()->QuadsBegin();
	vec2 Pos = mix(vec2(pPrev->m_X, pPrev->m_Y), vec2(pCurrent->m_X, pCurrent->m_Y), Client()->IntraGameTick());
	float Angle = 0.0f;
	float Size = 64.0f;
	if (pCurrent->m_Type == POWERUP_WEAPON)
	{
		Angle = 0; //-pi/6;//-0.25f * pi * 2.0f;
		RenderTools()->SelectSprite(g_pData->m_Weapons.m_aId[clamp(pCurrent->m_Subtype, 0, NUM_WEAPONS-1)].m_pSpriteBody);
		Size = g_pData->m_Weapons.m_aId[clamp(pCurrent->m_Subtype, 0, NUM_WEAPONS-1)].m_VisualSize;
		if(g_Config.m_ClEffectsWeapontrail)
		{
			if(pCurrent->m_Subtype == 2)
				m_pClient->m_pEffects->PowerupShine(Pos, vec2(64,32),vec4(0.25f,1,0.25f,1));
			else if(pCurrent->m_Subtype == 3)
				m_pClient->m_pEffects->PowerupShine(Pos, vec2(64,32),vec4(1,0.4f,0.4f,1));
			else if(pCurrent->m_Subtype == 4)
				m_pClient->m_pEffects->PowerupShine(Pos, vec2(64,32),vec4(0.3232f,0.03232f,1,1));
		}
	}
	else
	{
		const int c[] = {
			SPRITE_PICKUP_HEALTH,
			SPRITE_PICKUP_ARMOR,
			SPRITE_PICKUP_WEAPON,
			SPRITE_PICKUP_NINJA
			};
		RenderTools()->SelectSprite(c[pCurrent->m_Type]);

		if(c[pCurrent->m_Type] == SPRITE_PICKUP_NINJA)
		{
			m_pClient->m_pEffects->PowerupShine(Pos, vec2(96,18));
			Size *= 2.0f;
			Pos.x -= 10.0f;
		}
	}

	Graphics()->QuadsSetRotation(Angle);

	static float s_Time = 0.0f;
	static float s_LastLocalTime = Client()->LocalTime();
	float Offset = Pos.y/32.0f + Pos.x/32.0f;
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
	{
		const IDemoPlayer::CInfo *pInfo = DemoPlayer()->BaseInfo();
		if(!pInfo->m_Paused)
			s_Time += (Client()->LocalTime()-s_LastLocalTime)*pInfo->m_Speed;
	}
	else
	{
		if(m_pClient->m_Snap.m_pGameInfoObj && !(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_PAUSED))
			s_Time += Client()->LocalTime()-s_LastLocalTime;
 	}
	Pos.x += cosf(s_Time*2.0f+Offset)*2.5f;
	Pos.y += sinf(s_Time*2.0f+Offset)*2.5f;
	s_LastLocalTime = Client()->LocalTime();
	RenderTools()->DrawSprite(Pos.x, Pos.y, Size);
	Graphics()->QuadsEnd();
}

void CItems::RenderFlag(const CNetObj_Flag *pPrev, const CNetObj_Flag *pCurrent, const CNetObj_GameData *pPrevGameData, const CNetObj_GameData *pCurGameData)
{
	float Angle = 0.0f;
	float Size = 42.0f;

	Graphics()->BlendNormal();
	Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
	Graphics()->QuadsBegin();

	if(pCurrent->m_Team == TEAM_RED)
		RenderTools()->SelectSprite(SPRITE_FLAG_RED);
	else
		RenderTools()->SelectSprite(SPRITE_FLAG_BLUE);

	Graphics()->QuadsSetRotation(Angle);

	vec2 Pos = mix(vec2(pPrev->m_X, pPrev->m_Y), vec2(pCurrent->m_X, pCurrent->m_Y), Client()->IntraGameTick());

	if(pCurGameData)
	{
		// make sure that the flag isn't interpolated between capture and return
		if(pPrevGameData &&
			((pCurrent->m_Team == TEAM_RED && pPrevGameData->m_FlagCarrierRed != pCurGameData->m_FlagCarrierRed) ||
			(pCurrent->m_Team == TEAM_BLUE && pPrevGameData->m_FlagCarrierBlue != pCurGameData->m_FlagCarrierBlue)))
			Pos = vec2(pCurrent->m_X, pCurrent->m_Y);

		// make sure to use predicted position if we are the carrier
		if(m_pClient->m_Snap.m_pLocalInfo &&
			((pCurrent->m_Team == TEAM_RED && pCurGameData->m_FlagCarrierRed == m_pClient->m_Snap.m_LocalClientID) ||
			(pCurrent->m_Team == TEAM_BLUE && pCurGameData->m_FlagCarrierBlue == m_pClient->m_Snap.m_LocalClientID)))
			Pos = m_pClient->m_LocalCharacterPos;
	}

	if(pCurrent->m_Team == TEAM_RED) // red team
	{
		if(g_Config.m_ClEffectsFlagtrail && pCurGameData)
			if(pCurGameData->m_FlagCarrierRed == FLAG_ATSTAND || pCurGameData->m_FlagCarrierRed == FLAG_MISSING)
				m_pClient->m_pEffects->PowerupShine(Pos, vec2(32,32),vec4(0.89f,0.16f,0.21f,1));
	}
	else
	{
		if(g_Config.m_ClEffectsFlagtrail && pCurGameData)
			if(pCurGameData->m_FlagCarrierBlue == FLAG_ATSTAND || pCurGameData->m_FlagCarrierBlue == FLAG_MISSING)
				m_pClient->m_pEffects->PowerupShine(Pos, vec2(32,32),vec4(0.098f,0.10f,0.89f,1));
	}

	
	IGraphics::CQuadItem QuadItem(Pos.x, Pos.y-Size*0.75f, Size, Size*2);
	Graphics()->QuadsDraw(&QuadItem, 1);
	Graphics()->QuadsEnd();
}


void CItems::RenderLaser(const struct CNetObj_Laser *pCurrent)
{
	vec2 Pos = vec2(pCurrent->m_X, pCurrent->m_Y);
	vec2 From = vec2(pCurrent->m_FromX, pCurrent->m_FromY);
	vec2 Dir = normalize(Pos-From);

	float Ticks = Client()->GameTick() + Client()->IntraGameTick() - pCurrent->m_StartTick;
	float Ms = (Ticks/50.0f) * 1000.0f;
	float a = Ms / m_pClient->m_Tuning.m_LaserBounceDelay;
	a = clamp(a, 0.0f, 1.0f);
	float Ia = 1-a;

	vec2 Out, Border;

	Graphics()->BlendNormal();
	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();

	//vec4 inner_color(0.15f,0.35f,0.75f,1.0f);
	//vec4 outer_color(0.65f,0.85f,1.0f,1.0f);

	// do outline
	vec3 Rgb = m_pClient->m_pSkins->GetColorV3(g_Config.m_ClLaserColorOuter);
	Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f); // outline
	Out = vec2(Dir.y, -Dir.x) * (7.0f*Ia);

	IGraphics::CFreeformItem Freeform(
			From.x-Out.x, From.y-Out.y,
			From.x+Out.x, From.y+Out.y,
			Pos.x-Out.x, Pos.y-Out.y,
			Pos.x+Out.x, Pos.y+Out.y);
	Graphics()->QuadsDrawFreeform(&Freeform, 1);

	// do inner
	Out = vec2(Dir.y, -Dir.x) * (5.0f*Ia);
	Rgb = m_pClient->m_pSkins->GetColorV3(g_Config.m_ClLaserColorInner);
	Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f); // center

	Freeform = IGraphics::CFreeformItem(
			From.x-Out.x, From.y-Out.y,
			From.x+Out.x, From.y+Out.y,
			Pos.x-Out.x, Pos.y-Out.y,
			Pos.x+Out.x, Pos.y+Out.y);
	Graphics()->QuadsDrawFreeform(&Freeform, 1);

	Graphics()->QuadsEnd();

	// render head
	{
		Graphics()->BlendNormal();
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_PARTICLES].m_Id);
		Graphics()->QuadsBegin();

		int Sprites[] = {SPRITE_PART_SPLAT01, SPRITE_PART_SPLAT02, SPRITE_PART_SPLAT03};
		RenderTools()->SelectSprite(Sprites[Client()->GameTick()%3]);
		Graphics()->QuadsSetRotation(Client()->GameTick());
		Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f);
		IGraphics::CQuadItem QuadItem(Pos.x, Pos.y, 24, 24);
		Graphics()->QuadsDraw(&QuadItem, 1);
		Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f);
		QuadItem = IGraphics::CQuadItem(Pos.x, Pos.y, 20, 20);
		Graphics()->QuadsDraw(&QuadItem, 1);
		Graphics()->QuadsEnd();
	}

	Graphics()->BlendNormal();
}

void CItems::OnRender()
{
	if(Client()->State() < IClient::STATE_ONLINE)
		return;

	int Num = Client()->SnapNumItems(IClient::SNAP_CURRENT);
	for(int i = 0; i < Num; i++)
	{
		IClient::CSnapItem Item;
		const void *pData = Client()->SnapGetItem(IClient::SNAP_CURRENT, i, &Item);

		if(Item.m_Type == NETOBJTYPE_PROJECTILE)
		{
			RenderProjectile((const CNetObj_Projectile *)pData, Item.m_ID);
		}
		else if(Item.m_Type == NETOBJTYPE_PICKUP)
		{
			const void *pPrev = Client()->SnapFindItem(IClient::SNAP_PREV, Item.m_Type, Item.m_ID);
			if(pPrev)
				RenderPickup((const CNetObj_Pickup *)pPrev, (const CNetObj_Pickup *)pData);
		}
		else if(Item.m_Type == NETOBJTYPE_LASER)
		{
			RenderLaser((const CNetObj_Laser *)pData);
		}
	}

	// render flag
	for(int i = 0; i < Num; i++)
	{
		IClient::CSnapItem Item;
		const void *pData = Client()->SnapGetItem(IClient::SNAP_CURRENT, i, &Item);

		if(Item.m_Type == NETOBJTYPE_FLAG)
		{
			const void *pPrev = Client()->SnapFindItem(IClient::SNAP_PREV, Item.m_Type, Item.m_ID);
			if (pPrev)
			{
				const void *pPrevGameData = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_GAMEDATA, m_pClient->m_Snap.m_GameDataSnapID);
				RenderFlag(static_cast<const CNetObj_Flag *>(pPrev), static_cast<const CNetObj_Flag *>(pData),
							static_cast<const CNetObj_GameData *>(pPrevGameData), m_pClient->m_Snap.m_pGameDataObj);
			}
		}
	}

	// render extra projectiles
	for(int i = 0; i < m_NumExtraProjectiles; i++)
	{
		if(m_aExtraProjectiles[i].m_StartTick < Client()->GameTick())
		{
			m_aExtraProjectiles[i] = m_aExtraProjectiles[m_NumExtraProjectiles-1];
			m_NumExtraProjectiles--;
		}
		else
			RenderProjectile(&m_aExtraProjectiles[i], 0);
	}
}

void CItems::AddExtraProjectile(CNetObj_Projectile *pProj)
{
	if(m_NumExtraProjectiles != MAX_EXTRA_PROJECTILES)
	{
		m_aExtraProjectiles[m_NumExtraProjectiles] = *pProj;
		m_NumExtraProjectiles++;
	}
}
