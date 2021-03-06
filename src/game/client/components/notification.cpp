/*
#include <base/vmath.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include "notification.h"

CNotification::CNotification()
{
	m_Init = false;
	m_NowRendering = 0;
	m_Size = 0;
}


void NotifyAnimationDie(CNuiElements *Main, void *arg)
{
	//CNotification *pThis = (CNotification *)arg;
	//Main->SetLifeTime(0, 1);
	//Main->SetEndLifeAnimation(CValue::EaseIN6, vec4(210, 0, 0, 0));
}

void NotifyDie(CNuiElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;

	pThis->GetNotifyArray()->remove(Main);

	if(pThis->GetNotifyWaitArray()->size() > 0)
	{
		pThis->RenderNotification(pThis->GetNextNotify()->Type, pThis->GetNextNotify()->pTitle, pThis->GetNextNotify()->pText);
		pThis->GetNotifyWaitArray()->remove_index(0);
	}
	pThis->SetSize(pThis->GetNotifyWaitArray()->size());
	if(pThis->GetNotifyArray()->size() == 0)
	{
		CNuiElements *NotifyBox = pThis->ControllerNUI()->GetElement(CNuiElements::BLOCK, "Notification.NotifyBox");
		NotifyBox->GetPos()->Init(pThis->GetNotifyBoxStartPos());
	}
}

void NotifyBoxAnimEnd(CNuiElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	if(pThis->GetNotifyArray()->size() < 5 && pThis->GetNotifyWaitArray()->size() > 0)
	{
		if(pThis->GetNotifyWaitArray()->size() > 0)
			pThis->RenderNotification(pThis->GetNextNotify()->Type, pThis->GetNextNotify()->pTitle, pThis->GetNextNotify()->pText);

		pThis->GetNotifyWaitArray()->remove_index(0);
	}
	pThis->SetSize(pThis->GetNotifyWaitArray()->size());
}


bool CNotification::AddNew(NotificationType Type, const char *pTitle, const char *pText)
{
	if(m_aNotify.size() < 5)
	{
		CNuiElements *NotifyBox = ControllerNui()->GetElement(CNuiElements::BLOCK, "Notification.NotifyBox");
		NotifyBox->GetPos()->InitPlus(vec4(0, -70, 0, 70), 1, CValue::EaseOUT4);
		//NotifyBox->SetAnimationEndCallback(NotifyBoxAnimEnd, this);

		TempNames[m_NowRendering] = new char[64] {0};
		str_format(TempNames[m_NowRendering], 64, "Notification.NotifyBox.Notify%d", m_NowRendering);

		CNuiElements *Main = ControllerNui()->GetElement(CNuiElements::BLOCK, TempNames[m_NowRendering]);

		Main->GetPos()->Init(vec4(NotifyBox->GetPos()->GetValue().w+70, NotifyBox->GetPos()->GetValue().h, NotifyBox->GetPos()->GetValue().w, 60));
		Main->GetPos()->InitPlus(vec4(-(NotifyBox->GetPos()->GetValue().w+70), 0, 0, 0), 1, CValue::EaseOUTBounce);
		//Main->SetLifeTime(10, 1);
		Main->SetBlock(5.0f, CNuiElements::CORNER_ALL);
		Main->GetColor()->Init(vec4(0, 1, 0, 0.4f));
		//Main->SetEndLifeTimeCallback(NotifyAnimationDie, NotifyDie, this);
		//Main->SetCallbacksEvents(NotifyAnimationDie, NULL, NULL, this);

		TempTitle[m_NowRendering] = new char[64] {0};
		str_format(TempTitle[m_NowRendering], 64, "%s.Title", TempNames[m_NowRendering]);
		CNuiElements *Title = ControllerNui()->GetElement(CNuiElements::TEXT, TempTitle[m_NowRendering]);
		Title->GetPos()->Init(vec4(0, 0, NotifyBox->GetPos()->GetValue().w, 20));
		Title->SetText(false, CNuiElements::ALIGN_CENTER, "%s", pTitle);
		Title->GetColor()->Init(vec4(1, 1, 1, 1));
		//Title->SetEndLifeTimeCallback(NotifyAnimationDie, NULL, this);

		TempText[m_NowRendering] = new char[64] {0};
		str_format(TempText[m_NowRendering], 64, "%s.Text", TempNames[m_NowRendering]);
		CNuiElements *Text = ControllerNui()->GetElement(CNuiElements::TEXT, TempText[m_NowRendering]);
		Text->GetPos()->Init(vec4(0, 30, NotifyBox->GetPos()->GetValue().w, 12));
		Text->SetText(false, CNuiElements::ALIGN_CENTER, "%s", pText);
		Text->GetColor()->Init(vec4(1, 1, 1, 1));
		//Text->SetEndLifeTimeCallback(NotifyAnimationDie, NULL, this);

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
	SetSize(GetNotifyWaitArray()->size());
}

void MainMouseOver(CNuiElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->GetPos()->InitPlus(vec4(-10, -2, 0, 4), 1, CValue::EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, 0.3f), 1, CValue::EaseOUTElastic);

	CNuiElements *Text = pThis->ControllerNUI()->GetElement(CNuiElements::TEXT, "Notification1.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,0.5f), 1, CValue::EaseOUTElastic);
}

void MainMouseOut(CNuiElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;
	Main->GetPos()->InitPlus(vec4(10, 2, 0, -4), 1, CValue::EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, -0.3f), 1, CValue::EaseOUTElastic);

	CNuiElements *Text = pThis->ControllerNUI()->GetElement(CNuiElements::TEXT, "Notification1.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,-0.5f), 1, CValue::EaseOUTElastic);
}

void MainClick(CNuiElements *Main, void *arg)
{
	CNotification *pThis = (CNotification *)arg;

	if(pThis->GetNotifyWaitArray()->size() > 0)
	{
		pThis->RenderNotification(pThis->GetNextNotify()->Type, pThis->GetNextNotify()->pTitle, pThis->GetNextNotify()->pText);
		pThis->GetNotifyWaitArray()->remove_index(0);
	}
}


void CNotification::OnRender()
{

if(!m_Init)
	{
		m_Size = 0;
		CNuiElements *Main2 = ControllerNui()->GetElement(CNuiElements::BLOCK, "Notification1.Background");

		CUIRect Screen, MainRect, TextRect;
		Graphics()->GetScreen(&Screen.x, &Screen.y, &Screen.w, &Screen.h);
		//CUIRect Screen = *UI()->Screen();
		Screen.VSplitRight(50.0f, 0, &MainRect);
		MainRect.HSplitBottom(175.0f, &MainRect, 0);
		MainRect.HSplitBottom(50.0f, 0, &MainRect);

		Main2->GetPos()->Init(vec4(MainRect.x+MainRect.w, MainRect.y, MainRect.w, MainRect.h));
		Main2->GetPos()->Init(vec4(MainRect.x+10, MainRect.y, MainRect.w, MainRect.h), 1, CValue::EaseOUTElastic);
		Main2->SetBlock(5.0f, CNuiElements::CORNER_L);
		Main2->GetColor()->Init(vec4(0, 1, 0, 0.2f));
		Main2->SetCallbacksVisual(MainMouseOver, MainMouseOut, NULL, NULL, this);
		Main2->SetCallbacksEvents(MainClick, NULL, NULL, this);

		CNuiElements *Text = ControllerNui()->GetElement(CNuiElements::TEXT, "Notification1.Background.Text");
		Text->GetPos()->Init(vec4(0, 15, 50, 20.0f));
		Text->GetColor()->Init(vec4(1,1,1,0.5f));
		Text->SetText(true, CNuiElements::ALIGN_CENTER, "%d", &m_Size);


		m_NotifyBoxStartPos = vec4(MainRect.x-210, MainRect.y, MainRect.w+140, MainRect.h+20);
		CNuiElements *NotifyBox = ControllerNui()->GetElement(CNuiElements::BLOCK, "Notification.NotifyBox");
		NotifyBox->GetPos()->Init(m_NotifyBoxStartPos);
		if(g_Config.m_Debug)
			NotifyBox->GetColor()->Init(vec4(0, 0, 0, 0.2f));

		m_Init = true;
	}
}
*/

