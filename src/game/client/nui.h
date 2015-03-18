#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <base/tl/array.h>
#include <engine/graphics.h>
#include <engine/client.h>
#include "render.h"
#include "gameclient.h"

enum ANIMATION_TYPE
{
	LINEAR,
	EaseIN,				// плавно, вначале медленно, вконце быстро, общая скорость - очень медленно
	EaseOUT,			// плавно, вначале быстро, вконце медленно, общая скорость - очень медленно
	EaseINOUT,			// плавно, вначале и конце медленно, общая скорость - очень медленно
	EaseIN2,			// плавно, вначале медленно, вконце быстро, общая скорость - медленно
	EaseOUT2,			// плавно, вначале быстро, вконце медленно, общая скорость - медленно
	EaseINOUT2,			// плавно, вначале и конце медленно, общая скорость - медленно
	EaseIN3,			// плавно, вначале медленно, вконце быстро, общая скорость - средне
	EaseOUT3,			// плавно, вначале быстро, вконце медленно, общая скорость - средне
	EaseINOUT3,			// плавно, вначале и конце медленно, общая скорость - средне
	EaseIN4,			// плавно, вначале медленно, вконце быстро, общая скорость - быстро
	EaseOUT4,			// плавно, вначале быстро, вконце медленно, общая скорость - быстро
	EaseINOUT4,			// плавно, вначале и конце медленно, общая скорость - быстро
	EaseIN5,			// плавно, вначале медленно, вконце быстро, общая скорость - очень быстро
	EaseOUT5,			// плавно, вначале быстро, вконце медленно, общая скорость - очень быстро
	EaseINOUT5,			// плавно, вначале и конце медленно, общая скорость - очень быстро
	EaseINBack,			// плавно c запозданием, вначале запаздываем
	EaseOUTBack,		// плавно c запозданием, вконце запаздываем
	EaseINOUTBack,		// плавно c запозданием, вначале и вконце запаздываем
	EaseINElastic,		// плавно, вконце эластично
	EaseOUTElastic,		// плавно, вначале эластично
	EaseINOUTElastic,	// плавно, в центре эластично
	EaseINBounce,		// плавно, прыгает медленно
	EaseOUTBounce,		// плавно, сразу прыгает
	EaseINOUTBounce,	// плавно, прыгает и вначале и вконце
	Default,
};

enum RENDER_LEVEL
{
	LOWEST,
	LOW,
	NORMAL,
	HIGHT,
	HIGHTEST,
};

class CNUI
{
public:
	CNUI(class CGameClient *pClient, class CControllerNui *pControllerNui);

	void Render();
	void SetLifeTime(int LifeTime, float EndLifeDur = 1);
	void SetEndLife(float EndLifeDur = 1);
	void SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord);

	class IClient *Client() const;
	class IGraphics *Graphics() const;
	class CRenderTools *RenderTools() const;

	class CValue *GetPos() { return m_pPos; }
	class CValue *GetColor() { return m_pColor; }

	void SetRenderLevel(RENDER_LEVEL Level) {m_Renderlevel = Level;}
	RENDER_LEVEL GetRenderLevel() {return m_Renderlevel;}
private:
	bool m_DieProcess;
	bool m_EndLife;
	float m_EndLifeDur;
	double m_EndLifeTime;

	vec4 m_DieCoord;
	ANIMATION_TYPE m_DieAnimation;

	class CValue *m_pPos;
	class CValue *m_pColor;
	class CGameClient *m_pClient;
	RENDER_LEVEL m_Renderlevel;
	class CControllerNui *m_pControllerNui;
};

class CValue
{
public:
	CValue(CNUI *pNUI);
	void Init(vec4 value);
	void Init(vec4 value, float time, ANIMATION_TYPE animation_type);

	void Recalculate();
	void EndAnimation();

	bool m_AnimEnded;
	double m_AnimTime;
	double m_AnimEndTime;
	ANIMATION_TYPE m_Animation;
	vec4 m_Value, m_NewValue, m_OldValue;

private:
	CNUI *m_pNui;
	vec4 Animation(ANIMATION_TYPE anim, vec4 min, vec4 max, double time);
};

class CControllerNui
{
public:
	CControllerNui(class CGameClient *Client);

	struct SNUIElements
	{
		const char *Name;
		CNUI *Element;
	};

	CNUI *GetElement(const char *Name);
	int GetSize() {return m_aNui.size();}
	CNUI *GetElement(int num) {return m_aNui[num]->Element;}
	void RemoveElement(CNUI *pNui);
private:
	class CGameClient *m_pClient;
	array <SNUIElements *> m_aNui;
};

#endif //GAME_CLIENT_NUI_H
