#include <base/vmath.h>
#include <engine/graphics.h>
#include "notification.h"

CNotification::CNotification()
{
	m_Init = false;
	m_NowRendering = 0;
	m_Size = 0;
}

void NotifyAnimationDie(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->SetLifeTime(0, 1);
	Main->SetEndLifeAnimation(EaseIN6, vec4(210, 0, -140, 0));
}

void NotifyDie(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;

	pThis->GetNotifyArray()->remove(Main);

	if(pThis->GetNotifyWaitArray()->size() > 0)
		pThis->RenderNotification(pThis->GetNextNotify()->Type, pThis->GetNextNotify()->pTitle, pThis->GetNextNotify()->pText);

	pThis->GetNotifyWaitArray()->remove_index(0);
}

bool CNotification::AddNew(NotificationType Type, const char *pTitle, const char *pText)
{
	if(m_aNotify.size() < 5)
	{

		CNUIElements *NotifyBox = ControllerNui()->GetElement(ELEMENT_BLOCK, "Notification.NotifyBox");
		NotifyBox->GetPos()->InitPlus(vec4(0, -90, 0, 90), 1, EaseOUT4);
		TempNames[m_NowRendering] = new char[64] {0};
		str_format(TempNames[m_NowRendering], 64, "Notification.NotifyBox.Notify%d", m_NowRendering);
		//CUIRect Screen, MainRect, TextRect;

		CNUIElements *Main = ControllerNui()->GetElement(ELEMENT_BLOCK, TempNames[m_NowRendering]);
/*		Graphics()->GetScreen(&Screen.x, &Screen.y, &Screen.w, &Screen.h);
		Screen.VSplitRight(50.0f, 0, &MainRect);
		MainRect.HSplitBottom(175.0f, &MainRect, 0);
		MainRect.HSplitBottom(50.0f, 0, &MainRect);*/

		Main->GetPos()->Init(vec4(NotifyBox->GetPos()->m_Value.w+70, NotifyBox->GetPos()->m_Value.h, NotifyBox->GetPos()->m_Value.w, 70));
		//Main->GetPos()->Init(vec4(0, 0, NotifyBox->GetPos()->m_Value.w, NotifyBox->GetPos()->m_Value.h));
		Main->GetPos()->InitPlus(vec4(-(NotifyBox->GetPos()->m_Value.w+70), 0, 0, 0), 1, EaseOUTBounce);
		Main->SetLifeTime(10, 1);
		Main->SetBlock(5.0f, CORNER_ALL);
		Main->GetColor()->Init(vec4(0, 1, 0, 0.4f));
		//Main->GetPos()->InitPlus(vec4(-210, 0, 140, 20), 1, EaseOUT5);
		Main->SetEndLifeTimeCallback(NotifyAnimationDie, NotifyDie, this);

		TempTitle[m_NowRendering] = new char[64] {0};
		str_format(TempTitle[m_NowRendering], 64, "%s.Title", TempNames[m_NowRendering]);
		CNUIElements *Title = ControllerNui()->GetElement(ELEMENT_TEXT, TempTitle[m_NowRendering]);
		Title->GetPos()->Init(vec4(0, 0, NotifyBox->GetPos()->m_Value.w, 20));
		Title->SetText(false, ALIGN_CENTER, "%s", pTitle);
		Title->GetColor()->Init(vec4(1, 1, 1, 1));
		Title->SetEndLifeTimeCallback(NotifyAnimationDie, NULL, this);

		TempText[m_NowRendering] = new char[64] {0};
		str_format(TempText[m_NowRendering], 64, "%s.Text", TempNames[m_NowRendering]);
		CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, TempText[m_NowRendering]);
		Text->GetPos()->Init(vec4(0, 30, NotifyBox->GetPos()->m_Value.w, 12));
		Text->SetText(false, ALIGN_CENTER, "%s", pText);
		Text->GetColor()->Init(vec4(1, 1, 1, 1));
		Text->SetEndLifeTimeCallback(NotifyAnimationDie, NULL, this);

		m_aNotify.add(Main);
		m_NowRendering = (m_NowRendering+1)%5;
		return true;
	}
	else
	{
		SNotify Notify;
		Notify.Type = Type;
		Notify.pTitle = pTitle;
		Notify.pText = pText;
		m_aNotifyWait.add(Notify);
		return false;
	}
}

void CNotification::RenderNotification(NotificationType Type, const char *pTitle, const char *pText)
{
	AddNew(Type, pTitle, pText);

	m_Size = m_aNotifyWait.size();
}

void CNotification::Add(NotificationType Type, const char *pTitle, const char *pText)
{
	SNotify Notify;
	Notify.Type = Type;
	Notify.pTitle = pTitle;
	Notify.pText = pText;
	m_aNotifyWait.add(Notify);
}

void MainMouseOver(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->GetPos()->InitPlus(vec4(-10, -2, 0, 4), 1, EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, 0.3f), 1, EaseOUTElastic);

	CNUIElements *Text = pThis->ControllerNUI()->GetElement(ELEMENT_TEXT, "Notification.Box.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,0.5f), 1, EaseOUTElastic);
}

void MainMouseOut(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->GetPos()->InitPlus(vec4(10, 2, 0, -4), 1, EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, -0.3f), 1, EaseOUTElastic);

	CNUIElements *Text = pThis->ControllerNUI()->GetElement(ELEMENT_TEXT, "Notification.Box.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,-0.5f), 1, EaseOUTElastic);
}

void MainClick(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;

	for(int i = 0; i < pThis->GetNotifyWaitArray()->size(); i++)
	{
		if(i==5)
			break;
		pThis->RenderNotification(pThis->GetNextNotify()->Type, pThis->GetNextNotify()->pTitle, pThis->GetNextNotify()->pText);

		pThis->GetNotifyWaitArray()->remove_index(0);
	}
}

void CNotification::OnRender()
{
	if(!m_Init)
	{
		m_Size = 0;
		CNUIElements *Main = ControllerNui()->GetElement(ELEMENT_BLOCK, "Notification.Box.Background");

		CUIRect Screen, MainRect, TextRect;
		Graphics()->GetScreen(&Screen.x, &Screen.y, &Screen.w, &Screen.h);
		//CUIRect Screen = *UI()->Screen();
		Screen.VSplitRight(50.0f, 0, &MainRect);
		MainRect.HSplitBottom(175.0f, &MainRect, 0);
		MainRect.HSplitBottom(50.0f, 0, &MainRect);

		Main->GetPos()->Init(vec4(MainRect.x+MainRect.w, MainRect.y, MainRect.w, MainRect.h));
		Main->GetPos()->Init(vec4(MainRect.x+10, MainRect.y, MainRect.w, MainRect.h), 1, EaseOUTElastic);
		Main->SetBlock(5.0f, CORNER_L);
		Main->GetColor()->Init(vec4(0, 1, 0, 0.2f));
		Main->SetCallbacksVisual(MainMouseOver, MainMouseOut, NULL, NULL, this);
		Main->SetCallbacksEvents(MainClick, NULL, NULL, this);

		CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, "Notification.Box.Background.Text");
		Text->GetPos()->Init(vec4(0, 15, 50, 20.0f));
		Text->GetColor()->Init(vec4(1,1,1,0.5f));
		Text->SetText(true, ALIGN_CENTER, "%d", &m_Size);


		CNUIElements *NotifyBox = ControllerNui()->GetElement(ELEMENT_BLOCK, "Notification.NotifyBox");
		NotifyBox->GetPos()->Init(vec4(MainRect.x-210, MainRect.y, MainRect.w+140, MainRect.h+20));
		NotifyBox->GetColor()->Init(vec4(0, 0, 0, 0.2f));

		m_Init = true;
	}
}