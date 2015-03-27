
#ifndef GAME_CLIENT_UI_NELEMENTS_H
#define GAME_CLIENT_UI_NELEMENTS_H

#include <base/tl/array.h>
#include <game/client/nui.h>
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

    CNuiElements(CNui *pNui, const char *Name);

    virtual void Render();
    virtual void PreRender();
    virtual void PostRender();

    const char *m_pName;

    CNuiElements *m_pParent;
    array<CNuiElements *>m_apChild;

protected:
    CValue m_PosLocal;
    CValue m_Color;

    CNui *m_pNui;
};

#endif //GAME_CLIENT_UI_NELEMENTS_H
