#include <iostream>
#include <stdio.h>
#include <getopt.h>

#include "main.hpp"

using namespace std;

namespace Parser
{
	void usage()
	{
		printf(
		"\n"
		"Usage: navigna [options]\n\n"
		"\t-w/--width [number] \t: set window's width\n"
		"\t-h/--height [number] \t: set window's height\n"
		"\t-f/--fullscreen\t\t: be fullscreen\n"
		"\t-l/--no-split\t: don't split the screen\n"
		"\t-n/--network\t\t: run at network\n"
		"\t\t-s/--is-server\t: run as a server\n"
		"\t\t-r/--host\t: host ip\n"
		"\t\t-p/--port\t: host port\n\n"
		"NOTICE: This arguments can be changed/stored in the game later\n"
		"Navigna is free software.  See COPYING for details.\n"
		"\n"
		);
	}
	
	int parse_args(int argc, char** argv)
	{
	
		 static struct option long_options[] =
		{
			{"full-screen",	no_argument,				0,	'f'},
			{"network",			no_argument,				0,	'n'},
			{"width",				required_argument,	0,	'x'},
			{"height",			required_argument,	0,	'y'},
			{"is-server",		no_argument,				0,	's'},
			{"host",			required_argument,		0,	'r'},
			{"port",				required_argument,	0,	'p'},
			{"no-split", 		no_argument,				0,	'l'},
			{0, 0, 0, 0}
		};
		
		char option;
		int option_index = 0;
		
		while ((option = getopt_long(argc, argv, "fx:y:nhsr:p:l", long_options, &option_index)) != EOF )
		{
			switch (option) 
			{
				printf ("option %s", long_options[option_index].name);
				case 'f':
					FULLSCREEN = true;
					break;
				case 'x':
					WIDTH = atoi(optarg);
					break;
				case 'y':
					HEIGHT = atoi(optarg);
					break;
				case 'l':
				case 'n':
					isSplit = false;
					break;
				case 'r':
					host = optarg;
					break;
				case 'p':
					port = atoi(optarg);
					break;
				case 's':
					isServer = true;
					isClient = false;
				case 'h':
				case '?':
					usage();
					return 1;
			}
			
		}
		return 0;
	}
	
}
