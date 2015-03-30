#ifndef GAME_CLIENT_UI_NELEMENTS_H
#define GAME_CLIENT_UI_NELEMENTS_H

#include <base/tl/array.h>
#include "value.h"

class CNuiElements
{
public:

    enum ELEMENT_TYPE
    {
        BLOCK,
        TEXT,
        QUAD,
        TEE
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
	enum TEXT_ALIGN
	{
		ALIGN_CENTER,
		ALIGN_LEFT,
		ALIGN_RIGHT
	};

    CNuiElements(class CNui *pNui, const char *Name);
	~CNuiElements();

    virtual void Render() = 0;
    virtual void PreRender();
    virtual void PostRender();

	virtual void SetBlock(float RoundCorner, CORNER_TYPES Type){};
	virtual void SetText(bool TextUpdate, TEXT_ALIGN Align, const char *pText, ...){};

	void SetChildClipping();
	void DisableParentClipping(){m_StopClipByParent = true;}
	void EnableParentClipping(){m_StopClipByParent = false;}

	class IClient *Client() const;
	class IGraphics *Graphics() const;
	class ITextRender *TextRender() const;
	class CRenderTools *RenderTools() const;

	CValue *GetPos() { return m_pPosLocal; }
	CValue *GetColor() { return m_pColor; }
    char *m_pName;

    CNuiElements *m_pParent;
    array<CNuiElements *> m_apChild;

	vec4 GetChildPosGlobal() { return m_PosGlobal+m_pPosLocal->GetValue();}

protected:
	vec4 GetClipPos();
	bool GetClipEnable(){return m_ClipUsed;};
	vec4 GetClipWithoutScale();

	vec4 m_PosGlobal;
    CValue *m_pPosLocal;
    CValue *m_pColor;

    class CNui *m_pNui;

	bool m_StopClipByParent;
	bool m_ClipUsed;
	float m_XScale;
	float m_YScale;

};

#endif //GAME_CLIENT_UI_NELEMENTS_H
