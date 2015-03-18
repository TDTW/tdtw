#ifndef GAME_CLIENT_COMPONENTS_NOTIFICATION_H
#define GAME_CLIENT_COMPONENTS_NOTIFICATION_H
#include <game/client/component.h>
#include <base/tl/array.h>

enum NotificationType
{
	NT_DEFAULT,
	NT_IMPORTANT,
	NT_CHAT,
};

class CNotification : public CComponent
{
public:
	CNotification();

	void RenderNotification(NotificationType Type, const char *pTitle, const char *pText);
	virtual void OnRender() {};
private:
	struct Notify
	{
		NotificationType Type;
		const char *pTitle;
		const char *pText;
	};
	array<Notify *> m_aNotify;
};


#endif // GAME_CLIENT_COMPONENTS_NOTIFICATION_H

