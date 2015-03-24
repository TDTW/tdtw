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
	//Main->SetEndLifeTimeCallback(NULL, NotifyDie, arg);
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
		TempNames[m_NowRendering] = new char[64] {0};
		str_format(TempNames[m_NowRendering], 64, "Notification.Notify%d", m_NowRendering);
		CUIRect Screen, MainRect, TextRect;

		CNUIElements *Main = ControllerNui()->GetElement(ELEMENT_BLOCK, TempNames[m_NowRendering]);
		Graphics()->GetScreen(&Screen.x, &Screen.y, &Screen.w, &Screen.h);
		Screen.VSplitRight(50.0f, 0, &MainRect);
		MainRect.HSplitBottom(175.0f, &MainRect, 0);
		MainRect.HSplitBottom(50.0f, 0, &MainRect);

		Main->GetPos()->Init(vec4(MainRect.x, MainRect.y, MainRect.w, MainRect.h));
		Main->SetLifeTime(10, 1);
		Main->SetBlock(5.0f, CORNER_ALL);
		Main->GetColor()->Init(vec4(0, 1, 0, 0.4f));
		Main->GetPos()->InitPlus(vec4(-210, 0, 140, 20), 1, EaseOUT5);
		Main->SetEndLifeTimeCallback(NotifyAnimationDie, NotifyDie, this);

		TempTitle[m_NowRendering] = new char[64] {0};
		str_format(TempTitle[m_NowRendering], 64, "%s.Title", TempNames[m_NowRendering]);
		CNUIElements *Title = ControllerNui()->GetElement(ELEMENT_TEXT, TempTitle[m_NowRendering]);
		Title->GetPos()->Init(vec4(0, 0, MainRect.w+140, 20));
		Title->SetText(false, ALIGN_CENTER, "%s", pTitle);
		Title->GetColor()->Init(vec4(1, 1, 1, 1));
		Title->SetEndLifeTimeCallback(NotifyAnimationDie, NULL, this);

		TempText[m_NowRendering] = new char[64] {0};
		str_format(TempText[m_NowRendering], 64, "%s.Text", TempNames[m_NowRendering]);
		CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, TempText[m_NowRendering]);
		Text->GetPos()->Init(vec4(0, 30, MainRect.w+140, 12));
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
	if(AddNew(Type, pTitle, pText) && m_aNotify.size() > 1)
	{
		for(int i=0; i < m_aNotify.size()-1; i++)
		{
			m_aNotify[i]->GetPos()->InitPlus(vec4(0, -90, 0, 0), 1, EaseOUT4);
		}
	}
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

	CNUIElements *Text = pThis->ControllerNUI()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,0.5f), 1, EaseOUTElastic);
}

void MainMouseOut(CNUIElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->GetPos()->InitPlus(vec4(10, 2, 0, -4), 1, EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, -0.3f), 1, EaseOUTElastic);

	CNUIElements *Text = pThis->ControllerNUI()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
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
		CNUIElements *Main = ControllerNui()->GetElement(ELEMENT_BLOCK, "Main.Background");

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
		Main->SetRenderLevel(HIGHTEST);
		Main->SetCallbacksVisual(MainMouseOver, MainMouseOut, NULL, NULL, this);
		Main->SetCallbacksEvents(MainClick, NULL, NULL, this);

		CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
		Text->GetPos()->Init(vec4(0, 15, 50, 20.0f));
		Text->GetColor()->Init(vec4(1,1,1,0.5f));
		Text->SetText(false, ALIGN_CENTER, "%d", &m_Size);

		m_Init = true;
	}
}