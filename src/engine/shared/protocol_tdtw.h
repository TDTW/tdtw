/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_SHARED_PROTOCOL_TDTW_H
#define ENGINE_SHARED_PROTOCOL_TDTW_H

#include <base/system.h>


enum 
{
	NETMSG_TDTW_NULL = 0,
	NETMSG_TDTW_VERSION,
	NETMSG_TDTW_HASH_REQUEST,
	NETMSG_TDTW_HASH_DONE,
	NETMSG_TDTW_UPDATE_INFO,
	NETMSG_TDTW_UPDATE_DATA,
	NETMSG_TDTW_UPDATE_REQUEST,
};

#endif