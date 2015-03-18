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
	EaseIN,				// ������, ������� ��������, ������ ������, ����� �������� - ����� ��������
	EaseOUT,			// ������, ������� ������, ������ ��������, ����� �������� - ����� ��������
	EaseINOUT,			// ������, ������� � ����� ��������, ����� �������� - ����� ��������
	EaseIN2,			// ������, ������� ��������, ������ ������, ����� �������� - ��������
	EaseOUT2,			// ������, ������� ������, ������ ��������, ����� �������� - ��������
	EaseINOUT2,			// ������, ������� � ����� ��������, ����� �������� - ��������
	EaseIN3,			// ������, ������� ��������, ������ ������, ����� �������� - ������
	EaseOUT3,			// ������, ������� ������, ������ ��������, ����� �������� - ������
	EaseINOUT3,			// ������, ������� � ����� ��������, ����� �������� - ������
	EaseIN4,			// ������, ������� ��������, ������ ������, ����� �������� - ������
	EaseOUT4,			// ������, ������� ������, ������ ��������, ����� �������� - ������
	EaseINOUT4,			// ������, ������� � ����� ��������, ����� �������� - ������
	EaseIN5,			// ������, ������� ��������, ������ ������, ����� �������� - ����� ������
	EaseOUT5,			// ������, ������� ������, ������ ��������, ����� �������� - ����� ������
	EaseINOUT5,			// ������, ������� � ����� ��������, ����� �������� - ����� ������
	EaseINBack,			// ������ c �����������, ������� �����������
	EaseOUTBack,		// ������ c �����������, ������ �����������
	EaseINOUTBack,		// ������ c �����������, ������� � ������ �����������
	EaseINElastic,		// ������, ������ ���������
	EaseOUTElastic,		// ������, ������� ���������
	EaseINOUTElastic,	// ������, � ������ ���������
	EaseINBounce,		// ������, ������� ��������
	EaseOUTBounce,		// ������, ����� �������
	EaseINOUTBounce,	// ������, ������� � ������� � ������
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
