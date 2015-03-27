#include <game/client/nui.h>
#include "elements.h"

CNuiElements::CNuiElements(CNui *pNui, const char *Name)
{
    m_pName = new char[] (Name);
    m_pNui = pNui;
    m_pParent = NULL;
}
