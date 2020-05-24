#pragma once

#include <ctime>
#include <functional>
#include <memory>
#include <string>

static constexpr unsigned QUIT = 0;
static constexpr unsigned TURN = 1;
static constexpr unsigned MOVE_F = 2;
static constexpr unsigned MOVE_B = 3;
static constexpr unsigned SHOT = 4;
static constexpr unsigned UP = 5;
static constexpr unsigned DOWN = 6;
static constexpr unsigned LEFT = 7;
static constexpr unsigned RIGHT = 8;
static constexpr unsigned SPIN_L = 9;
static constexpr unsigned SPIN_R = 10;
static constexpr unsigned COMMAND_BEGIN = 0;
static constexpr unsigned COMMAND_END = 1;
static constexpr unsigned JOIN_GAME = 11;
static constexpr unsigned NEW_SHIP = 12;
static constexpr unsigned INIT_POS = 13;
static constexpr unsigned UPDATE_SHIP = 14;
static constexpr unsigned SHIP_POS = 15;

static constexpr unsigned PROTOCOL_SIZE = sizeof(protocol);
static constexpr unsigned MAX_NAME_LEN = 16; //get from another place

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;

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

