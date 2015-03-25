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
	struct SNotify
	{
		NotificationType Type;
		const char *pTitle;
		const char *pText;
	};
public:
	CNotification();
	void Add(NotificationType Type, const char *pTitle, const char *pText);
	void RenderNotification(NotificationType Type, const char *pTitle, const char *pText);
	virtual void OnRender();
	class CControllerNui *ControllerNUI() { return ControllerNui(); }
	int m_NowRendering;
	array<CNUIElements *> *GetNotifyArray() { return &m_aNotify; }
	array<SNotify> *GetNotifyWaitArray() {return &m_aNotifyWait;}
	SNotify *GetNextNotify() {return &m_aNotifyWait[0];}
	void SetSize(int Size){m_Size = Size;}
	vec4 GetNotifyBoxStartPos() {return m_NotifyBoxStartPos; }
private:
	bool m_Init;
	array<CNUIElements *> m_aNotify;
	array<SNotify> m_aNotifyWait;
	char *TempNames[5];
	char *TempTitle[5];
	char *TempText[5];
	int m_Size;
	vec4 m_NotifyBoxStartPos;
	bool AddNew(NotificationType Type, char const *pTitle, char const *pText);
};


#endif // GAME_CLIENT_COMPONENTS_NOTIFICATION_H

