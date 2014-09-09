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

Messages = [

	NetMessage("AutoUpdate",[
        NetString("m_WantUpdate"),
	], True),
]