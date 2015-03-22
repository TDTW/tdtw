#include <engine/graphics.h>
#include "notification.h"

CNotification::CNotification()
{
	m_aNotify.clear();
	m_Init = false;
}

void CNotification::RenderNotification(NotificationType Type, const char *pTitle, const char *pText)
{
	Notify *pNotify = new Notify;
	pNotify->Type = Type;
	pNotify->pTitle = pTitle;
	pNotify->pText = pText;
	m_aNotify.add(pNotify);
	dbg_msg("test", "%s: %s", pTitle, pText);
}

void OnMouseOver(CNUIElements *Main)
{
	Main->GetPos()->InitPlus(vec4(-10, 0, 0, 0), 1, EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, 0.3f), 1, EaseOUTElastic);

	CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,0.5f), 1, EaseOUTElastic);
}

void OnMouseOut(CNUIElements *Main)
{
	Main->GetPos()->InitPlus(vec4(10, 0, 0, 0), 1, EaseOUTElastic);
	Main->GetColor()->InitPlus(vec4(0, 0, 0, -0.3f), 1, EaseOUTElastic);

	CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
	Text->GetColor()->InitPlus(vec4(0,0,0,-0.5f), 1, EaseOUTElastic);
}

void CNotification::OnRender()
{
	if(m_Init == false)
	{
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

		CNUIElements *Text = ControllerNui()->GetElement(ELEMENT_TEXT, "Main.Background.Text");
		Text->GetPos()->Init(vec4(0, 15, 50, 20.0f));
		Text->GetColor()->Init(vec4(1,1,1,0.5f));
		Text->SetText(false, ALIGN_CENTER, "%d", 0);
		Main->SetCallbacksVisual(OnMouseOver,OnMouseOut,NULL,NULL);

		m_Init = true;
	}
}