#ifndef GAME_CLIENT_UI_NELEMENTS_H
#define GAME_CLIENT_UI_NELEMENTS_H
#include <base/vmath.h>

enum RENDER_LEVEL
{
	LOWEST,
	LOW,
	NORMAL,
	HIGHT,
	HIGHTEST,
};
enum TEXT_ALIGN
{
	ALIGN_CENTER,
	ALIGN_LEFT,
	ALIGN_RIGHT
};
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
enum CORNER_TYPES
{
	CORNER_TL = 1,
	CORNER_TR = 2,
	CORNER_BL = 4,
	CORNER_BR = 8,

	CORNER_T = CORNER_TL | CORNER_TR,
	CORNER_B = CORNER_BL | CORNER_BR,
	CORNER_R = CORNER_TR | CORNER_BR,
	CORNER_L = CORNER_TL | CORNER_BL,

	CORNER_ALL = CORNER_T | CORNER_B
};

class CNUIElements
{
public:
	const char *m_pName;

	CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name);

	virtual void Render()
	{
	};

	virtual void SetQuad()
	{
	};

	virtual void SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...)
	{
	};

	virtual void SetBlock(float RoundCorner, CORNER_TYPES Type)
	{
	};

	virtual void PreRender();

	virtual void PostRender();

	void SetLifeTime(int LifeTime, float EndLifeDur = 1);
	void SetEndLife(float EndLifeDur = 1);
	void SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord);

	class IClient *Client() const;
	class IGraphics *Graphics() const;

	class ITextRender *TextRender() const;
	class CRenderTools *RenderTools() const;

	class CValue *GetPos() { return m_pPosLocal; }
	class CValue *GetColor() { return m_pColor; }

	virtual class CValue *GetColorOutline()
	{
	};
	vec4 GetChildPosGlobal();
	void SetRenderLevel(RENDER_LEVEL Level) {m_Renderlevel = Level;}
	RENDER_LEVEL GetRenderLevel() {return m_Renderlevel;}

	CNUIElements *m_pParent;
protected:
	bool m_DieProcess;
	bool m_EndLife;
	float m_EndLifeDur;
	double m_EndLifeTime;

	vec4 m_DieCoord;
	ANIMATION_TYPE m_DieAnimation;

	vec4 m_pPosGlobal;
	class CValue *m_pPosLocal;
	class CValue *m_pColor;
	class CGameClient *m_pClient;
	RENDER_LEVEL m_Renderlevel;
	class CControllerNui *m_pControllerNui;
};

#endif //GAME_CLIENT_UI_NELEMENTS_H
