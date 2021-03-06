/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_MENUS_H
#define GAME_CLIENT_COMPONENTS_MENUS_H

#include <base/vmath.h>
#include <base/tl/sorted_array.h>

#include <engine/demo.h>
#include <engine/friends.h>

#include <game/voting.h>
#include <game/client/component.h>
#include <game/client/ui.h>


// compnent to fetch keypresses, override all other input
class CMenusKeyBinder : public CComponent
{
public:
	bool m_TakeKey;
	bool m_GotKey;
	IInput::CEvent m_Key;
	CMenusKeyBinder();
	virtual bool OnInput(IInput::CEvent Event);
};

class CMenus : public CComponent
{
	static vec4 ms_GuiColor;
	static vec4 ms_ColorTabbarInactiveOutgame;
	static vec4 ms_ColorTabbarActiveOutgame;
	static vec4 ms_ColorTabbarInactiveIngame;
	static vec4 ms_ColorTabbarActiveIngame;
	static vec4 ms_ColorTabbarInactive;
	static vec4 ms_ColorTabbarActive;

	vec4 ButtonColorMul(const void *pID);

	float *ButtonFade(const void *pID, const void *pValue, float Seconds, int Checked=0);
	float *ButtonFade(const void *pID, const void *pValue, const CUIRect *pRect, float Seconds);
	float *ButtonFadeActive(const void *pID, const void *pValueFade, float Seconds, int Checked=0);

	int DoButton_DemoPlayer(const void *pID, const char *pText, int Checked, const CUIRect *pRect);
	int DoButton_Sprite(const void *pID, int ImageID, int SpriteID, int Checked, const CUIRect *pRect, int Corners);
	int DoButton_Toggle(const void *pID, int Checked, const CUIRect *pRect, bool Active);
	int DoButton_Menu(const void *pID, const char *pText, int Checked, const CUIRect *pRect, int Corners = CUI::CORNER_ALL);
	int DoButton_MenuTab(const void *pID, const char *pText, int Checked, const CUIRect *pRect, int Corners);

	int DoButton_CheckBox_Common(const void *pID, const void *s_CheckBox, const char *pText, const char *pBoxText, int Checked, const CUIRect *pRect);
	int DoButton_CheckBox(const void *pID, const void *s_CheckBox, const char *pText, int Checked, const CUIRect *pRect);
	int DoButton_CheckBox_Number(const void *pID, const void *s_CheckBox, const char *pText, int Checked, const CUIRect *pRect);

	/*static void ui_draw_menu_button(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	static void ui_draw_keyselect_button(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	static void ui_draw_menu_tab_button(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	static void ui_draw_settings_tab_button(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	*/

	int DoButton_Icon(int ImageId, int SpriteId, const CUIRect *pRect);
	int DoButton_GridHeader(const void *pID, const char *pText, int Checked, const CUIRect *pRect, int Sort);

	//static void ui_draw_browse_icon(int what, const CUIRect *r);
	//static void ui_draw_grid_header(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);

	/*static void ui_draw_checkbox_common(const void *id, const char *text, const char *boxtext, const CUIRect *r, const void *extra);
	static void ui_draw_checkbox(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	static void ui_draw_checkbox_number(const void *id, const char *text, int checked, const CUIRect *r, const void *extra);
	*/
	int DoEditBox(void *pID, const void *pValueFade, const CUIRect *pRect, char *pStr, unsigned StrSize, float FontSize, float *Offset, bool Hidden=false, int Corners=CUI::CORNER_ALL);
	//static int ui_do_edit_box(void *id, const CUIRect *rect, char *str, unsigned str_size, float font_size, bool hidden=false);

	float DoScrollbarV(const void *pID, const float *pFade, const CUIRect *pRect, float Current, int SizeSlider = 33);
	float DoScrollbarH(const void *pID, const float *pFade, const CUIRect *pRect, float Current);
	int DoCoolScrollbarH(const void *pID, const float *pFade, const CUIRect *pRect, int Real, float Min, float Max, int ShowNumber = 1);
	void DoButton_KeySelect(const void *pID, const char *pText, int Checked, const CUIRect *pRect, int Corners=CUI::CORNER_ALL);
	int DoKeyReader(void *pID, const CUIRect *pRect, int Key, int Corners=CUI::CORNER_ALL);

	//static int ui_do_key_reader(void *id, const CUIRect *rect, int key);
	void UiDoGetButtons(int Start, int Stop, CUIRect View, bool SpecButton = false);

	struct CListboxItem
	{
		int m_Visible;
		int m_Selected;
		CUIRect m_Rect;
		CUIRect m_HitRect;
	};

	void UiDoListboxStart(const void *pID, const float *pFade, const CUIRect *pRect, float RowHeight, const char *pTitle, const char *pBottomText, int NumItems,
						int ItemsPerRow, int SelectedIndex, float ScrollValue, int TopCorners = CUI::CORNER_T, int BottomCorners = CUI::CORNER_B);
	CListboxItem UiDoListboxNextItem(const void *pID, bool Selected = false);
	CListboxItem UiDoListboxNextItem(const void *pID, const float *pFadeValue, bool Selected = false, vec3 BloodColor = vec3(1.0f,1.0f,1.0f));
	CListboxItem UiDoListboxNextRow();
	int UiDoListboxEnd(float *pScrollValue, bool *pItemActivated);

	//static void demolist_listdir_callback(const char *name, int is_dir, void *user);
	//static void demolist_list_callback(const CUIRect *rect, int index, void *user);

	enum
	{
		POPUP_NONE=0,
		POPUP_FIRST_LAUNCH,
		POPUP_CONNECTING,
		POPUP_MESSAGE,
		POPUP_DISCONNECTED,
		POPUP_PURE,
		POPUP_LANGUAGE,
		POPUP_COUNTRY,
		POPUP_DELETE_DEMO,
		POPUP_RENAME_DEMO,
		POPUP_REMOVE_FRIEND,
		POPUP_SOUNDERROR,
		POPUP_PASSWORD,
		POPUP_QUIT,
	};

	enum
	{
		PAGE_NEWS=1,
		PAGE_GAME,
		PAGE_PLAYERS,
		PAGE_SERVER_INFO,
		PAGE_CALLVOTE,
		PAGE_INTERNET,
		PAGE_LAN,
		PAGE_FAVORITES,
		PAGE_DEMOS,
		PAGE_SETTINGS,
		PAGE_SYSTEM,
		PAGE_BROWSER,
	};

	int m_GamePage;
	int m_Popup;
	int m_ActivePage;
	int m_ActivePage2;
	bool m_MenuActive;
	bool m_UseMouseButtons;
	vec2 m_MousePos;
	
	char s_aName[MAX_NAME_LENGTH];
	char s_aClan[MAX_CLAN_LENGTH];
	int64 m_LastInput;

	// loading
	int m_LoadCurrent;
	int m_LoadTotal;

	//
	char m_aMessageTopic[512];
	char m_aMessageBody[512];
	char m_aMessageButton[512];

	void PopupMessage(const char *pTopic, const char *pBody, const char *pButton);

	// TODO: this is a bit ugly but.. well.. yeah
	enum { MAX_INPUTEVENTS = 32 };
	static IInput::CEvent m_aInputEvents[MAX_INPUTEVENTS];
	static int m_NumInputEvents;

	// some settings
	static float ms_ButtonHeight;
	static float ms_ListheaderHeight;
	static float ms_FontmodHeight;
	
	float TempSens;

	// for settings
	bool m_NeedRestartGraphics;
	bool m_NeedRestartSound;
	bool m_NeedSendinfo;
	int m_SettingPlayerPage;

	//
	bool m_EscapePressed;
	bool m_EnterPressed;
	bool m_DeletePressed;

	// for map download popup
	int64 m_DownloadLastCheckTime;
	int m_DownloadLastCheckSize;
	float m_DownloadSpeed;

	// for call vote
	int m_CallvoteSelectedOption;
	int m_CallvoteSelectedPlayer;
	char m_aCallvoteReason[VOTE_REASON_LENGTH];

	// demo
	struct CDemoItem
	{
		char m_aFilename[128];
		char m_aName[128];
		bool m_IsDir;
		int m_StorageType;

		bool m_InfosLoaded;
		bool m_Valid;
		CDemoHeader m_Info;

		bool operator<(const CDemoItem &Other) { return !str_comp(m_aFilename, "..") ? true : !str_comp(Other.m_aFilename, "..") ? false :
														m_IsDir && !Other.m_IsDir ? true : !m_IsDir && Other.m_IsDir ? false :
														str_comp_filenames(m_aFilename, Other.m_aFilename) < 0; }
	};

	sorted_array<CDemoItem> m_lDemos;
	char m_aCurrentDemoFolder[256];
	char m_aCurrentDemoFile[64];
	int m_DemolistSelectedIndex;
	bool m_DemolistSelectedIsDir;
	int m_DemolistStorageType;

	void DemolistOnUpdate(bool Reset);
	void DemolistPopulate();
	static int DemolistFetchCallback(const char *pName, int IsDir, int StorageType, void *pUser);

	// friends
	struct CFriendItem
	{
		const CFriendInfo *m_pFriendInfo;
		int m_NumFound;

		bool operator<(const CFriendItem &Other)
		{
			if(m_NumFound && !Other.m_NumFound)
				return true;
			else if(!m_NumFound && Other.m_NumFound)
				return false;
			else
			{
				int Result = str_comp(m_pFriendInfo->m_aName, Other.m_pFriendInfo->m_aName);
				if(Result)
					return Result < 0;
				else
					return str_comp(m_pFriendInfo->m_aClan, Other.m_pFriendInfo->m_aClan) < 0;
			}
		}
	};

	sorted_array<CFriendItem> m_lFriends;
	int m_FriendlistSelectedIndex;
	int64 _my_rtime;

	void FriendlistOnUpdate();

	// found in menus.cpp
	int Render();
	//void render_background();
	//void render_loading(float percent);
	int RenderMenubar(CUIRect r);
	void RenderNews(CUIRect MainView);

	// found in menus_demo.cpp
	void RenderDemoPlayer(CUIRect MainView);
	void RenderDemoList(CUIRect MainView);

	// found in menus_ingame.cpp
	void RenderGame(CUIRect MainView);
	void RenderPlayers(CUIRect MainView);
	void RenderServerInfo(CUIRect MainView);
	void RenderServerControl(CUIRect MainView);
	void RenderServerControlKick(CUIRect MainView, bool FilterSpectators);
	void RenderServerControlServer(CUIRect MainView);
	void RenderIngameServerbrowser(CUIRect MainView);
	
	// found in menus_browser.cpp
	int m_SelectedIndex;
	int m_ScrollOffset;
	void RenderServerbrowserServerList(CUIRect View);
	void RenderServerbrowserServerDetail(CUIRect View);
	void RenderServerbrowserFilters(CUIRect View);
	void RenderServerbrowserFriends(CUIRect View);
	void RenderServerbrowser(CUIRect MainView, bool Playing = false);
	static void ConchainFriendlistUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);
	static void ConchainServerbrowserUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	// found in menus_settings.cpp
	void RenderFontSelection(CUIRect MainView);
	void RenderLanguageSelection(CUIRect MainView);
	void RenderSettingsGeneral(CUIRect MainView);
	void RenderSettingsGame(CUIRect MainView);
	void RenderSettingsHudMod(CUIRect MainView);
	void RenderSettingsTee(CUIRect MainView);
	void RenderSettingsControls(CUIRect MainView);
	void RenderSettingsGraphics(CUIRect MainView);
	void RenderSettingsSound(CUIRect MainView);
	void RenderSettingsCountry(CUIRect View);
	void RenderSettings(CUIRect MainView);
	void RenderColFeat(CUIRect MainView);
	void RenderColHud(CUIRect MainView);
	void KeyBinder();
	
	void SetActive(bool Active);
public:
	void RenderBackground();

	void UseMouseButtons(bool Use) { m_UseMouseButtons = Use; }

	static CMenusKeyBinder m_Binder;

	CMenus();

	void RenderLoading();

	bool IsActive() const { return m_MenuActive; }

	virtual void OnInit();

	virtual void OnStateChange(int NewState, int OldState);
	virtual void OnReset();
	virtual void OnRender();
	virtual bool OnInput(IInput::CEvent Event);
	virtual bool OnMouseMove(float x, float y);
};
#endif
