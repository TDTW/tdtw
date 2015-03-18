#include "notification.h"

CNotification::CNotification()
{
	m_aNotify.clear();
}

void CNotification::RenderNotification(NotificationType Type, const char *pTitle, const char *pText)
{
	Notify *pNotify = new Notify;
	pNotify->Type = Type;
	pNotify->pTitle = pTitle;
	pNotify->pText = pText;
	m_aNotify.add(pNotify);
}
