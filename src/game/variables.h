/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_VARIABLES_H
#define GAME_VARIABLES_H
#undef GAME_VARIABLES_H // this file will be included several times

#define TDTW_IP "62.221.112.98:8000"
#define TDTW_PORT 8000

// client
MACRO_CONFIG_INT(ClPredict, cl_predict, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Predict client movements")
MACRO_CONFIG_INT(ClNameplates, cl_nameplates, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show name plates")
MACRO_CONFIG_INT(ClClanplates, cl_clanplates, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show clan plates")
MACRO_CONFIG_INT(ClNameplatesAlways, cl_nameplates_always, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Always show name plates disregarding of distance")
MACRO_CONFIG_INT(ClNameplatesTeamcolors, cl_nameplates_teamcolors, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Use team colors for name plates")
MACRO_CONFIG_INT(ClNameplatesSize, cl_nameplates_size, 50, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Size of the name plates from 0 to 100%")
MACRO_CONFIG_INT(ClClanplatesScale, cl_clanplates_scale, 70, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Scale factor of the clan plates from 0 to 100%")
MACRO_CONFIG_INT(ClAutoswitchWeapons, cl_autoswitch_weapons, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Auto switch weapon on pickup")

MACRO_CONFIG_INT(ClShowhud, cl_showhud, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame HUD")
MACRO_CONFIG_INT(ClShowChatFriends, cl_show_chat_friends, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show only chat messages from friends")
MACRO_CONFIG_INT(ClShowfps, cl_showfps, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame FPS counter")

MACRO_CONFIG_INT(ClAirjumpindicator, cl_airjumpindicator, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClThreadsoundloading, cl_threadsoundloading, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Load sound files threaded")

MACRO_CONFIG_INT(ClWarningTeambalance, cl_warning_teambalance, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Warn about team balance")

MACRO_CONFIG_INT(ClMouseDeadzone, cl_mouse_deadzone, 0, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseFollowfactor, cl_mouse_followfactor, 0, 0, 200, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseMaxDistance, cl_mouse_max_distance, 400, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(EdShowkeys, ed_showkeys, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

//MACRO_CONFIG_INT(ClFlow, cl_flow, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClShowWelcome, cl_show_welcome, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMotdTime, cl_motd_time, 10, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "How long to show the server message of the day")

MACRO_CONFIG_STR(ClVersionServer, cl_version_server, 100, "version.teeworlds.com", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Server to use to check for new versions")

MACRO_CONFIG_STR(ClLanguagefile, cl_languagefile, 255, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "What language file to use")

MACRO_CONFIG_INT(PlayerUseCustomColor, player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toggles usage of custom colors")
MACRO_CONFIG_INT(PlayerColorBody, player_color_body, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player body color")
MACRO_CONFIG_INT(PlayerColorFeet, player_color_feet, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player feet color")
MACRO_CONFIG_STR(PlayerSkin, player_skin, 24, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player skin")

MACRO_CONFIG_INT(UiPage, ui_page, 6, 0, 10, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface page")
MACRO_CONFIG_INT(UiToolboxPage, ui_toolbox_page, 0, 0, 2, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toolbox page")
MACRO_CONFIG_STR(UiServerAddress, ui_server_address, 64, "localhost:8303", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface server address")
MACRO_CONFIG_INT(UiScale, ui_scale, 100, 50, 150, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface scale")
MACRO_CONFIG_INT(UiMousesens, ui_mousesens, 100, 5, 100000, CFGFLAG_SAVE|CFGFLAG_CLIENT, "Mouse sensitivity for menus/editor")

MACRO_CONFIG_INT(UiColorHue, ui_color_hue, 160, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color hue")
MACRO_CONFIG_INT(UiColorSat, ui_color_sat, 70, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color saturation")
MACRO_CONFIG_INT(UiColorLht, ui_color_lht, 175, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color lightness")
MACRO_CONFIG_INT(UiColorAlpha, ui_color_alpha, 228, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface alpha")

MACRO_CONFIG_INT(GfxNoclip, gfx_noclip, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disable clipping")

// server
MACRO_CONFIG_INT(ClMouseDeadzone2, cl_mouse_deadzone_2, 300, 0, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseFollowfactor2, cl_mouse_followfactor_2, 60, 0, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseMaxDistance2, cl_mouse_max_distance_2, 800, 0, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(SvWarmup, sv_warmup, 0, 0, 0, CFGFLAG_SERVER, "Number of seconds to do warmup before round starts")
MACRO_CONFIG_STR(SvMotd, sv_motd, 900, "", CFGFLAG_SERVER, "Message of the day to display for the clients")
MACRO_CONFIG_INT(SvTeamdamage, sv_teamdamage, 0, 0, 1, CFGFLAG_SERVER, "Team damage")
MACRO_CONFIG_STR(SvMaprotation, sv_maprotation, 768, "", CFGFLAG_SERVER, "Maps to rotate between")
MACRO_CONFIG_INT(SvRoundsPerMap, sv_rounds_per_map, 1, 1, 100, CFGFLAG_SERVER, "Number of rounds on each map before rotating")
MACRO_CONFIG_INT(SvRoundSwap, sv_round_swap, 1, 0, 1, CFGFLAG_SERVER, "Swap teams between rounds")
MACRO_CONFIG_INT(SvPowerups, sv_powerups, 1, 0, 1, CFGFLAG_SERVER, "Allow powerups like ninja")
MACRO_CONFIG_INT(SvScorelimit, sv_scorelimit, 20, 0, 1000, CFGFLAG_SERVER, "Score limit (0 disables)")
MACRO_CONFIG_INT(SvTimelimit, sv_timelimit, 0, 0, 1000, CFGFLAG_SERVER, "Time limit in minutes (0 disables)")
MACRO_CONFIG_STR(SvGametype, sv_gametype, 32, "dm", CFGFLAG_SERVER, "Game type (dm, tdm, ctf)")
MACRO_CONFIG_INT(SvTournamentMode, sv_tournament_mode, 0, 0, 1, CFGFLAG_SERVER, "Tournament mode. When enabled, players joins the server as spectator")
MACRO_CONFIG_INT(SvSpamprotection, sv_spamprotection, 1, 0, 1, CFGFLAG_SERVER, "Spam protection")

MACRO_CONFIG_INT(SvRespawnDelayTDM, sv_respawn_delay_tdm, 3, 0, 10, CFGFLAG_SERVER, "Time needed to respawn after death in tdm gametype")

MACRO_CONFIG_INT(SvSpectatorSlots, sv_spectator_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of slots to reserve for spectators")
MACRO_CONFIG_INT(SvTeambalanceTime, sv_teambalance_time, 1, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before autobalancing teams")
MACRO_CONFIG_INT(SvInactiveKickTime, sv_inactivekick_time, 3, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before taking care of inactive players")
MACRO_CONFIG_INT(SvInactiveKick, sv_inactivekick, 1, 0, 2, CFGFLAG_SERVER, "How to deal with inactive players (0=move to spectator, 1=move to free spectator slot/kick, 2=kick)")

MACRO_CONFIG_INT(SvStrictSpectateMode, sv_strict_spectate_mode, 0, 0, 1, CFGFLAG_SERVER, "Restricts information in spectator mode")
MACRO_CONFIG_INT(SvVoteSpectate, sv_vote_spectate, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to move players to spectators")
MACRO_CONFIG_INT(SvVoteSpectateRejoindelay, sv_vote_spectate_rejoindelay, 3, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before a player can rejoin after being moved to spectators by vote")
MACRO_CONFIG_INT(SvVoteKick, sv_vote_kick, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to kick players")
MACRO_CONFIG_INT(SvVoteKickMin, sv_vote_kick_min, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Minimum number of players required to start a kick vote")
MACRO_CONFIG_INT(SvVoteKickBantime, sv_vote_kick_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time to ban a player if kicked by vote. 0 makes it just use kick")

// TDTW
MACRO_CONFIG_STR(ClFontfile, cl_fontfile, 255, "DejaVu Sans Condensed.ttf", CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "What font file to use")
MACRO_CONFIG_INT(ZoomMax, zoom_max, 160, 110, 500, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Zoom maximum")
MACRO_CONFIG_INT(ZoomMin, zoom_min, 40, 20, 90, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Zoom minimum")

MACRO_CONFIG_INT(ReconnectEnable, reconnect_enable, 0, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Reconnect")
MACRO_CONFIG_INT(ReconnectBanTimeout, reconnect_ban_timeout, 10, 1, 120, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "How many seconds to wait before reconnecting (when banned)")
MACRO_CONFIG_INT(ReconnectFullTimeout, reconnect_full_timeout, 3, 1, 120, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "How many seconds to wait before reconnecting (when server is full)")

MACRO_CONFIG_INT(ShowGhost, cl_show_ghost, 0, 0, 2, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Show ghost")
MACRO_CONFIG_INT(AntiPing, cl_antiping, 0, 0, 2, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Antiping")
MACRO_CONFIG_INT(AntiPingTeeColor, cl_antiping_tee_color, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Antiping")
MACRO_CONFIG_INT(AntiPingGrenade, cl_antiping_grenade, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Antiping")
MACRO_CONFIG_INT(PlayerColorGhost, player_color_ghost, 65408, 0, 0xFFFFFF, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(PlayerColorGhostAlpha, player_color_ghost_alpha, 228, 0, 255, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "")

MACRO_CONFIG_INT(ClStandartHud, cl_standart_hud, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "standart hud")
MACRO_CONFIG_INT(ClHudShowAmmo, cl_hud_show_ammo, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Show ammo")
MACRO_CONFIG_INT(ClHudShowWeapon, cl_hud_show_weapon, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Show weapon")

MACRO_CONFIG_INT(ClChatShowtime, cl_chat_showtime, 15, 5, 60, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Time to show message")
MACRO_CONFIG_INT(ClChatHeightlimit, cl_chat_heightlimit, 80, 10, 248, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Chat height limit")

MACRO_CONFIG_INT(ClEffectsFlagtrail, cl_effects_flagtrail, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "FlagTrail")
MACRO_CONFIG_INT(ClEffectsWeapontrail, cl_effects_weapontrail, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "WeaponTrail")

MACRO_CONFIG_INT(ClHighlightPlayer, cl_highlight_player, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Highlight for number of players")
MACRO_CONFIG_INT(ClHighlightPing, cl_highlight_ping, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Highlight for ping")
MACRO_CONFIG_INT(ClHighlightGametypes, cl_highlight_gametypes, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Highlight for gametypes")
MACRO_CONFIG_INT(ClHighlightWeaponBar, cl_highlight_weaponbar, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Highlight for weapons bar")
MACRO_CONFIG_INT(ClBulletWeaponBar, cl_bullet_weaponbar, 1, 0, 1, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Show num of bullets for weapons bar")

MACRO_CONFIG_INT(ClLaserColorInner, cl_laser_color_inner, 11665217, 0, 16777215, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Laser color inner")
MACRO_CONFIG_INT(ClLaserColorOuter, cl_laser_color_outer, 11665217, 0, 16777215, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Laser color outer")

MACRO_CONFIG_INT(HudModHideAll, hudmod_hide_all, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide all hud")
MACRO_CONFIG_INT(HudModHideChat, hudmod_hide_chat, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide chat")
MACRO_CONFIG_INT(HudModHideKillMessages, hudmod_hide_kill_messages, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide kill messages")
MACRO_CONFIG_INT(HudModHideHealthAndAmmo, hudmod_hide_health_and_ammo, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide health and ammo")
MACRO_CONFIG_INT(HudModHideGameTimer, hudmod_hide_game_timer, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide game timer")
MACRO_CONFIG_INT(HudModHideSuddenDeath, hudmod_hide_sudden_death, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide sudden death")
MACRO_CONFIG_INT(HudModHideScoreHud, hudmod_hide_score_hud, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide score hud")
MACRO_CONFIG_INT(HudModHideWarmupTimer, hudmod_hide_warmup_timer, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide warmup timer")
MACRO_CONFIG_INT(HudModHideTeambalanceWarning, hudmod_hide_teambalance_warning, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide teambalance warning")
MACRO_CONFIG_INT(HudModHideVoting, hudmod_hide_voting, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide voting")
MACRO_CONFIG_INT(HudModHideCursor, hudmod_hide_cursor, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide cursor")
MACRO_CONFIG_INT(HudModHideBroadcast, hudmod_hide_broadcast, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide broadcast")
MACRO_CONFIG_INT(HudModHideSpectator, hudmod_hide_spectator, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide spectator")
MACRO_CONFIG_INT(HudModHideScorboardWhenDie, hudmod_hide_scorboard_when_die, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide Scorboard when die")
MACRO_CONFIG_INT(HudModHideBdadash, hudmod_hide_bdadash, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVETDTW, "Hide bdadash")

MACRO_CONFIG_INT(UiPage2, ui_page2, 6, 0, 11, CFGFLAG_SAVETDTW|CFGFLAG_CLIENT, "Interface page2")


// debug
#ifdef CONF_DEBUG // this one can crash the server if not used correctly
	MACRO_CONFIG_INT(DbgDummies, dbg_dummies, 0, 0, 15, CFGFLAG_SERVER, "")
#endif

MACRO_CONFIG_INT(DbgFocus, dbg_focus, 0, 0, 1, CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(DbgTuning, dbg_tuning, 0, 0, 1, CFGFLAG_CLIENT, "")
#endif
