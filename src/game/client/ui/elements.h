#ifndef GAME_CLIENT_UI_NELEMENTS_H
#define GAME_CLIENT_UI_NELEMENTS_H

#include <base/tl/array.h>
#include "value.h"
#include "block.h"
#include "text.h"

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

    CNuiElements(class CNui *pNui, const char *Name);
	~CNuiElements();

    virtual void Render() = 0;
    virtual void PreRender();
    virtual void PostRender();

	virtual void SetBlock(float RoundCorner, CBlock::CORNER_TYPES Type) = 0;
	virtual void SetText(bool TextUpdate, CText::TEXT_ALIGN Align, const char *pText, ...) = 0;

	class IClient *Client() const;
	class IGraphics *Graphics() const;
	class ITextRender *TextRender() const;
	class CRenderTools *RenderTools() const;

    const char *m_pName;

    CNuiElements *m_pParent;
    array<CNuiElements *> m_apChild;

	vec4 GetChildPosGlobal() { return m_PosGlobal+m_pPosLocal->GetValue();}

protected:
	vec4 m_PosGlobal;
    CValue *m_pPosLocal;
    CValue *m_pColor;

    class CNui *m_pNui;
};

#endif //GAME_CLIENT_UI_NELEMENTS_H
