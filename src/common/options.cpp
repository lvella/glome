#include "options.hpp"

#include <cstdlib>
#include <cstdio>
#include <getopt.h>

#include <SDL.h>

using namespace std;

namespace Options
{

#ifdef NDEBUG
#define DEBUG_OPT const
#else
#define DEBUG_OPT
#endif

// Debug options
DEBUG_OPT int numBots = 0;
DEBUG_OPT bool showBotScreen = false;
// TODO: numLocalPlayers...

// Video options
bool fullscreen = false;
int width = 800;
int height = 600;
bool vr_enable = false;

// Network options
// TODO: Make use of these:
bool isServer = false;
bool isClient = false;
std::string host;
short port = 0;

// TODO: this should be specific to SDL version
void usage()
{
	puts(
			"\n"
			"Usage: glome [options]\n\n"
#ifndef NDEBUG
			"Debug options:\n"
			"\t--bot-count [number] \t: how many bots to add\n"
			"\t--bot-screen \t: enable view of bot screen (at most 3 bots are shown)\n"
#endif
			"Video options:\n"
			"\t-x/--width [number] \t: set window's width\n"
			"\t-y/--height [number] \t: set window's height\n"
			"\t-f/--fullscreen\t\t: be fullscreen\n"
			"Networking options:\n"
			"\t-h/--host [ip]\t\t: host ip\n"
			"\t-s/--is-server\t\t: run as a server\n"
			"\t-c/--is-client\t\t: run as a client\n"
			"\t-p/--port [number]\t: host port\n"
			"\t-r/--vr\t\t\t: enable VR\n\n"
			//TODO: Make the following true: "NOTICE: This arguments can be changed/stored in the game later\n"
			"Glome is free software.  See COPYING for details.\n"
	);
}

int parse_args(int argc, char** argv)
{
	static struct option long_options[] =
	{
#ifndef NDEBUG
			{"bot-count",   required_argument,	0,	'b'},
			{"bot-screen",  no_argument,		0,	'v'},
#endif
			{"full-screen",	no_argument,		0,	'f'},
			{"width",		required_argument,	0,	'x'},
			{"height",		required_argument,	0,	'y'},
			{"is-server",	no_argument,		0,	's'},
			{"is-client",	no_argument,		0,	'c'},
			{"host",		required_argument,	0,	'h'},
			{"port",		required_argument,	0,	'p'},
			{"no-split",	no_argument,		0,	'l'},
			{"vr",			no_argument, 		0,	'r'},
			{0, 0, 0, 0}
	};

	char option;
	int option_index = 0;

	while ((option = getopt_long(argc, argv, "fx:y:csh:p:r", long_options, &option_index)) != EOF)
	{
		switch (option)
		{
#ifndef NDEBUG
		printf ("option %s", long_options[option_index].name);
		case 'b':
			numBots = atoi(optarg);
			break;
		case 'v':
			showBotScreen = true;
			break;
#endif
		case 'f':
			fullscreen = true;
			break;
		case 'x':
			width = atoi(optarg);
			break;
		case 'y':
			height = atoi(optarg);
			break;
		case 'h':
			host = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'c':
			isServer = false;
			isClient = true;
			break;
		case 's':
			isServer = true;
			isClient = false;
			break;
		case 'r':
			vr_enable = true;
			break;
		case '?':
			usage();
			return 1;
		}
	}

	// Check for incompatibilities
	if(showBotScreen && vr_enable)
	{
		fprintf(stderr, "Error : Can't show more than one player in VR mode.");
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Error",
			"Can't show more than one player in VR mode.",
			NULL);
		return 1;
	}

	return 0;
}



}

