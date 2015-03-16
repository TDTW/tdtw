#ifndef GAME_CLIENT_NUI_H
#define GAME_CLIENT_NUI_H

#include <base/tl/array.h>
#include <engine/graphics.h>
#include <engine/client.h>
#include "render.h"

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
	CNUI(class CGameClient *pClient);

	void Render();

	class IClient *Client() const;
	class IGraphics *Graphics() const;
	class CRenderTools *RenderTools() const;

	class CPos *GetPos() { return m_Pos; }
	void SetRenderLevel(RENDER_LEVEL Level) {m_Renderlevel = Level;}
	RENDER_LEVEL GetRenderLevel() {return m_Renderlevel;}
private:
	class CPos *m_Pos;
	class CGameClient *m_pClient;
	RENDER_LEVEL m_Renderlevel;
};

class CPos
{
	struct sCoord
	{
		float x;
		float y;
		float w;
		float h;
	};

public:
	CPos(CNUI *pNUI);
	void Init(float x, float y, float w, float h);
	void Init(float x, float y, float w, float h, float time, ANIMATION_TYPE animation_type);

	void Recalculate();

	double m_AnimTime;
	double m_AnimEndTime;
	ANIMATION_TYPE m_Animation;
	sCoord m_Coord, m_NewCoord, m_OldCoord;

private:
	CNUI *m_pNui;
	float Animation(ANIMATION_TYPE anim, float min, float max, double time);
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
private:
	class CGameClient *m_pClient;
	array <SNUIElements *> m_aNui;
	void SortArray();
};

#endif //GAME_CLIENT_NUI_H
