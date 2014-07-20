from datatypes import *

PlayerFlags = ["TDTW_JOYSTICK", "TDTW_SPECTATOR", "TDTW_FOLLOWER", "TDTW_PLAYING", "TDTW_IN_MENU", "TDTW_CHATTING", "TDTW_AFK"]


RawHeader = '''

#include <engine/message.h>
'''

RawSource = '''
#include <engine/message.h>
#include "protocol_tdtw.h"
'''

Flags = [
	Flags("PLAYERFLAG", PlayerFlags)
]

#Objects = [
   # NetObject("TestVersion",[
	#    NetStringStrict("m_Version"),
	#], True),
	#NetObject("PlayerInfo", [
	#	NetIntRange("m_Local", 0, 1),
	#	NetIntRange("m_ClientID", 0, 'MAX_CLIENTS-1'),
	#	NetIntRange("m_Team", 'TEAM_SPECTATORS', 'TEAM_BLUE'),

	#	NetIntAny("m_Score"),
	#	NetIntAny("m_Latency"),
	#]),
#]

Messages = [
	NetMessage("TestChat",[
		NetString("m_Name"),
		NetStringStrict("m_pMessage"),
	], True, True),
	NetMessage("Version",[
		NetString("m_Version"),
	], True, True),
	NetMessage("AutoUpdate",[
		NetString("m_Path"),
		NetIntAny("m_Size"),
	], True, True),
	#NetMessage("Sv_Chat", [
	#	NetIntRange("m_Team", 'TEAM_SPECTATORS', 'TEAM_BLUE'),
	#	NetIntRange("m_ClientID", -1, 'MAX_CLIENTS-1'),
	#	NetStringStrict("m_pMessage"),
	#]),

]
