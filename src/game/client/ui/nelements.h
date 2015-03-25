#ifndef GAME_CLIENT_UI_NELEMENTS_H
#define GAME_CLIENT_UI_NELEMENTS_H
#include <base/vmath.h>
#include "value.h"
enum RENDER_LEVEL
{
	LOWEST = 0,
	LOW,
	NORMAL,
	HIGHT,
	HIGHTEST,
	MOUSE,
};
enum TEXT_ALIGN
{
	ALIGN_CENTER,
	ALIGN_LEFT,
	ALIGN_RIGHT
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

typedef void (*CallBack)(class CNUIElements *Element, void *Arg);

class CNUIElements
{
public:
	const char *m_pName;

	CNUIElements(class CGameClient *pClient, class CControllerNui *pControllerNui, const char *Name);

	virtual void Render(){};
	virtual void SetQuad(){	};
	virtual void SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...)	{};
	virtual void SetBlock(float RoundCorner, CORNER_TYPES Type){};

	void SetCallbacksVisual(CallBack FocusOn, CallBack FocusOut, CallBack MouseDown, CallBack MouseUp, void *Arg);
	void SetCallbacksEvents(CallBack Click, CallBack DblClick, CallBack RightClick, void *Arg);
	void SetEndLifeTimeCallback(CallBack EndTime, CallBack BeforeDie, void *Arg);
	void SetAnimationEndCallback(CallBack End, void *Arg);
	void SetLifeTime(int LifeTime, float EndLifeDur = 1);
	void SetEndLife(float EndLifeDur = 1);
	void SetEndLifeAnimation(ANIMATION_TYPE animation_type, vec4 Coord);

	virtual void PreRender();
	virtual void PostRender();

	class IClient *Client() const;
	class IGraphics *Graphics() const;
	class ITextRender *TextRender() const;
	class CRenderTools *RenderTools() const;

	class CValue *GetPos() { return &m_PosLocal; }
	class CValue *GetColor() { return &m_Color; }

	virtual class CValue *GetColorOutline()	{};
	void SetChildClipping();
	void DisableParentClipping(){m_StopClipByParent = true;}
	void EnableParentClipping(){m_StopClipByParent = false;}
	vec4 GetClipPos();
	bool GetClipEnable(){return m_ClipUsed;};
	vec4 GetChildPosGlobal(){return m_PosGlobal + m_PosLocal.m_Value;};

	void SetRenderLevel(RENDER_LEVEL Level);
	RENDER_LEVEL GetRenderLevel() {return m_Renderlevel;}

	CNUIElements *m_pParent;
protected:

	bool m_StopClipByParent;
	bool m_ClipUsed;
	float m_XScale;
	float m_YScale;

	bool m_DieProcess;
	bool m_EndLife;
	float m_EndLifeDur;
	double m_EndLifeTime;

	vec4 m_DieCoord;
	ANIMATION_TYPE m_DieAnimation;

	vec4 m_PosGlobal;
	CValue m_PosLocal;
	CValue m_Color;

	RENDER_LEVEL m_Renderlevel;

	class CGameClient *m_pClient;
	class CControllerNui *m_pControllerNui;
private:
	bool m_Focus;
	CallBack m_FocusOn;
	CallBack m_FocusOut;
	CallBack m_MouseDown;
	CallBack m_MouseUp;
	//Event
	CallBack m_Click;
	CallBack m_DblClick;
	CallBack m_RightClick;

	//EndLifeTime
	CallBack m_EndLifeTimeCallback;
	bool m_EndLifeTimeCB;
	CallBack m_BeforeDieCallback;
	void *m_pEndLifeTimeArg;

	//Animation
	CallBack m_AnimEnd;
	void *m_pAnimArg;

	void *m_pVisualArg;
	void *m_pEventArg;

	bool m_UseVisualMouse;
	bool m_UseEventMouse;

	void CheckMouseVisual();
	void CheckMouseEvent();
	bool MouseInside();

	vec4 GetClipWithoutScale();
};

#endif //GAME_CLIENT_UI_NELEMENTS_H
