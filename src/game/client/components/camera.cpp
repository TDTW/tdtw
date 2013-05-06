/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>

#include <base/math.h>
#include <game/collision.h>
#include <game/client/gameclient.h>
#include <game/client/component.h>

#include "camera.h"
#include "controls.h"

CCamera::CCamera()
{
	m_CamType = CAMTYPE_UNDEFINED;
	m_Zoom = 1.0f;
	m_TempZoom = 1.0f;
}

void CCamera::ConKeyZoomIn(IConsole::IResult *pResult, void *pUserData)
{
	if(((CCamera *)pUserData)->Client()->State() == IClient::STATE_DEMOPLAYBACK || ((CCamera *)pUserData)->m_pClient->m_Snap.m_SpecInfo.m_Active)
		((CCamera *)pUserData)->m_TempZoom = clamp(((CCamera *)pUserData)->m_TempZoom-0.05f, (float)g_Config.m_ZoomMin/100, (float)g_Config.m_ZoomMax/100);
}

void CCamera::ConKeyZoomOut(IConsole::IResult *pResult, void *pUserData)
{
	if(((CCamera *)pUserData)->Client()->State() == IClient::STATE_DEMOPLAYBACK || ((CCamera *)pUserData)->m_pClient->m_Snap.m_SpecInfo.m_Active)
		((CCamera *)pUserData)->m_TempZoom = clamp(((CCamera *)pUserData)->m_TempZoom+0.05f, (float)g_Config.m_ZoomMin/100, (float)g_Config.m_ZoomMax/100);
}

void CCamera::ConZoomReset(IConsole::IResult *pResult, void *pUserData)
{
	((CCamera *)pUserData)->m_TempZoom = 1.0f;
}

void CCamera::OnConsoleInit()
{
	Console()->Register("+zoomin", "", CFGFLAG_CLIENT, ConKeyZoomIn, this, "");
	Console()->Register("+zoomout", "", CFGFLAG_CLIENT, ConKeyZoomOut, this, "");
	Console()->Register("zoomreset", "", CFGFLAG_CLIENT, ConZoomReset, this, "");
}

void CCamera::OnRender()
{
	//vec2 center;
	if(!m_pClient->m_Snap.m_SpecInfo.m_Active && Client()->State() != IClient::STATE_DEMOPLAYBACK)	// check zoom
		m_Zoom = 1.0f;
	
	if(m_TempZoom + 0.005f < m_Zoom)
		m_Zoom -= 0.01f;
	else if(m_TempZoom - 0.005f > m_Zoom)
		m_Zoom += 0.01f;

	// update camera center
	if(m_pClient->m_Snap.m_SpecInfo.m_Active && !m_pClient->m_Snap.m_SpecInfo.m_UsePosition)
	{
		if(m_CamType != CAMTYPE_SPEC)
		{
			m_pClient->m_pControls->m_MousePos = m_PrevCenter;
			m_pClient->m_pControls->ClampMousePos();
			m_CamType = CAMTYPE_SPEC;
		}
		m_Center = m_pClient->m_pControls->m_MousePos;
	}
	else
	{
		if(m_CamType != CAMTYPE_PLAYER)
		{
			m_pClient->m_pControls->ClampMousePos();
			m_CamType = CAMTYPE_PLAYER;
		}

		vec2 CameraOffset(0, 0);

		float l = length(m_pClient->m_pControls->m_MousePos);
		if(l > 0.0001f) // make sure that this isn't 0
		{
			float DeadZone = g_Config.m_ClMouseDeadzone;
			float FollowFactor = g_Config.m_ClMouseFollowfactor/100.0f;
			float OffsetAmount = max(l-DeadZone, 0.0f) * FollowFactor;

			CameraOffset = normalize(m_pClient->m_pControls->m_MousePos)*OffsetAmount;
		}

		if(m_pClient->m_Snap.m_SpecInfo.m_Active)
			m_Center = m_pClient->m_Snap.m_SpecInfo.m_Position + CameraOffset;
		else			
		{
			if (g_Config.m_ShowGhost == 2)
			{
				CNetObj_Character & Prev = m_pClient->m_Snap.m_aCharacters[m_pClient->m_Snap.m_LocalClientID].m_Prev;
				CNetObj_Character & Player = m_pClient->m_Snap.m_aCharacters[m_pClient->m_Snap.m_LocalClientID].m_Cur;
				vec2 Position = mix(vec2(Prev.m_X, Prev.m_Y), vec2(Player.m_X, Player.m_Y), Client()->IntraGameTick());

				m_Center = Position + CameraOffset;
			}
			else
			{
				m_Center = m_pClient->m_LocalCharacterPos + CameraOffset;
			}
		}
	}

	m_PrevCenter = m_Center;
}
