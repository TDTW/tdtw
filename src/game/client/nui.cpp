#include "nui.h"
#include "gameclient.h"

CNUI::CNUI(class CGameClient *pClient)
{
	m_pClient = pClient;
	m_Pos = new CPos(this);
	m_Renderlevel = NORMAL;
}


void CNUI::Render()
{
	CUIRect Screen = *m_pClient->UI()->Screen();
	Graphics()->MapScreen(Screen.x, Screen.y, Screen.w, Screen.h);

	if(m_Pos->m_AnimTime < Client()->GameTick() && Client()->GameTick() < m_Pos->m_AnimEndTime)
		m_Pos->Recalculate();

	Graphics()->TextureSet(-1);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 0.8f);
	RenderTools()->DrawRoundRectExt(m_Pos->m_Coord.x, m_Pos->m_Coord.y, m_Pos->m_Coord.w, m_Pos->m_Coord.h, 5.0f, CUI::CORNER_ALL);
	Graphics()->QuadsEnd();
}

CPos::CPos(CNUI *pNUI)
{
	m_pNui = pNUI;
}

void CPos::Init(float x, float y, float w, float h)
{
	this->m_Coord.x = x;
	this->m_Coord.y = y;
	this->m_Coord.w = w;
	this->m_Coord.h = h;

};

void CPos::Init(float x, float y, float w, float h, float time, ANIMATION_TYPE animation_type)
{

	m_NewCoord.x = x;
	m_NewCoord.y = y;
	m_NewCoord.w = w;
	m_NewCoord.h = h;
	m_OldCoord = m_Coord;


	m_Animation = animation_type;
	m_AnimTime = m_pNui->Client()->GameTick();
	m_AnimEndTime = m_pNui->Client()->GameTick() + (int)round(m_pNui->Client()->GameTickSpeed() * time);
}

void CPos::Recalculate()
{
	double PassedTime = (m_pNui->Client()->GameTick() -m_AnimTime) / ((m_AnimEndTime - m_AnimTime) * 1.0f);

	m_Coord.x = Animation(m_Animation, m_OldCoord.x, m_NewCoord.x, PassedTime);
	m_Coord.y = Animation(m_Animation, m_OldCoord.y, m_NewCoord.y, PassedTime);
	m_Coord.w = Animation(m_Animation, m_OldCoord.w, m_NewCoord.w, PassedTime);
	m_Coord.h = Animation(m_Animation, m_OldCoord.h, m_NewCoord.h, PassedTime);
}

float CPos::Animation(ANIMATION_TYPE anim, float min, float max, double time)
{
	switch (anim)
	{
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
		/*case BALL:
			float d=sinf((time-1.0f/12.0f)*M_PI*6.0f)*0.65f+0.65f;
			time=d+sinf(time*M_PI*0.5f)*(1.0f-d);
			break;*/
		/*case STEPS:
			time=(int)(time*7.0f)/7.0f;
			break;*/
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
			break;
		default:
			break;
	}
	return (float)(min + ((max-min) * time));
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
	CNUI *Temp = new CNUI(m_pClient);
	SNUIElements *NewElement = new SNUIElements;
	NewElement->Name = Name;
	NewElement->Element = Temp;
	m_aNui.add(NewElement);
	return NewElement->Element;
}

void CControllerNui::SortArray()
{

}

class IClient *CNUI::Client() const{return m_pClient->Client();}
class IGraphics *CNUI::Graphics() const{return m_pClient->Graphics();}
class CRenderTools *CNUI::RenderTools() const{return m_pClient->RenderTools();}
