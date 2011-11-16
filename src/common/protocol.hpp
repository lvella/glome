#pragma once

#include <ctime>
#include <functional>
#include <memory>
#include <string>

#define PROTOCOL_SIZE sizeof(protocol)
#define MAX_NAME_LEN 16 //get from another place

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

struct protocol
{
	uchar msg_type;
	clock_t time;
	union
	{
		// Player sending input to server or server broadcasting player input to the others (necessary for enemy prediction)
		struct
		{
			ushort ship_id;
			bool quit :1;
			bool up :1;
			bool down :1;
			bool left :1;
			bool right :1;
			bool movef :1;
			bool moveb :1;
			bool spinl :1;
			bool spinr :1;
			bool shoot :1;
			:6;
			ushort motionx;
			ushort motiony;
		} input;
		
		// Someone requests info about server (game mode, qtt of players etc) (used in lobby)
		struct
		{
			char server_name[MAX_NAME_LEN];
			uchar game_mode;
			ushort players;
			ushort max_players;
			bool priv;
		} info;

		// Any dynamic: player, asteroid etc
		struct
		{
			ushort dyn_id;
			uchar dyn_type;
			float m[4][4];
		} dynamic;
	} payload;
};

