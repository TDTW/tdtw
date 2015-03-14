#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <engine/graphics.h>
#include <base/vmath.h>
#include "gameclient.h"

class CNUI
{
protected:
	friend class CGameClient;

	CGameClient *m_pClient;
	class IClient *Client() const { return m_pClient->Client(); }
	class IGraphics *Graphics() const { return m_pClient->Graphics(); }
	class CRenderTools *RenderTools() const { return m_pClient->RenderTools(); }

public:
	enum ANIMATION_TYPE
	{
		LINEAR,
		FADEIN,
		FADEOUT,
		BALL,
		SIN,
		INCENTER,
		FADEBOTH,
		FADEBOTH2,
		STEPS,
		LAMP,
		JUMPIN,
		JUMPOUT
	};
private:
	class cPos
	{
		friend class CNUI;
		CNUI *m_pCNUI;

	private:
		struct sCoord
		{
			float x, y;
			float w, h;
		} Coord, newCoord, oldCoord;

		double time;
		double endTime;
		ANIMATION_TYPE animation;

		float Animation(ANIMATION_TYPE anim, float min, float max, double time)
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
					break;
				default:
					break;
			}
			return (float)(min + ((max-min) * time));
		}

		void Recalculate()
		{
			double PassedTime = (m_pCNUI->Client()->GameTick() - this->time) / ((this->endTime - this->time) * 1.0f);

			this->Coord.x = Animation(animation, this->oldCoord.x, this->newCoord.x, PassedTime);
			this->Coord.y = Animation(animation, this->oldCoord.y, this->newCoord.y, PassedTime);
			this->Coord.w = Animation(animation, this->oldCoord.w, this->newCoord.w, PassedTime);
			this->Coord.h = Animation(animation, this->oldCoord.h, this->newCoord.h, PassedTime);

		}

	public:
		cPos (float x, float y, float w, float h)
		{
			this->Coord = {x,y,w,h};
		}

		cPos (float x, float y, float w, float h, float time, ANIMATION_TYPE animation_type)
		{
			this->newCoord = {x,y,w,h};
			this->oldCoord = this->Coord;

			this->animation = animation_type;
			this->time = m_pCNUI->Client()->GameTick();
			this->endTime = m_pCNUI->Client()->GameTick() + (int)round(m_pCNUI->Client()->GameTickSpeed() * time);
		}
	} Pos;

	void Render()
	{
		if(this->Pos.time > 0.0f)
			this->Pos.Recalculate();

		Graphics()->TextureSet(-1);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);
		RenderTools()->DrawRoundRectExt(this->Pos.Coord.x, this->Pos.Coord.y, this->Pos.Coord.w, this->Pos.Coord.h, 5.0f, CUI::CORNER_ALL);
		Graphics()->QuadsEnd();
	}
};


#endif //GAME_CLIENT_NUI_H
