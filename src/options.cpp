#include <cstdlib>
#include <stdio.h>
#include <getopt.h>

#include "options.hpp"

using namespace std;

namespace Options
{
  const float FOV = 45.0f;
  int width = 800;
  int height = 600;
  std::string host;
  short port = 0;

  bool fullscreen = false;
  bool isServer = false;
  bool isClient = false;
  bool isSplit = true;

	void usage()
	{
		puts(
		"\n"
		"Usage: navigna [options]\n\n"
		"\t-w/--width [number] \t: set window's width\n"
		"\t-h/--height [number] \t: set window's height\n"
		"\t-f/--fullscreen\t\t: be fullscreen\n"
		"\t-l/--no-split\t\t: don't split the screen\n"
		"\t-h/--host [ip]\t\t: host ip\n"
		"\t-s/--is-server\t\t: run as a server\n"
		"\t-c/--is-client\t\t: run as a client\n"
		"\t-p/--port [number]\t: host port\n\n"
		"NOTICE: This arguments can be changed/stored in the game later\n"
		"Navigna is free software.  See COPYING for details.\n"
		);
	}
	
	int parse_args(int argc, char** argv)
	{
		bool net = 0;
		 static struct option long_options[] =
		{
			{"full-screen",	no_argument,				0,	'f'},
			{"network",			no_argument,				0,	'n'},
			{"width",				required_argument,	0,	'x'},
			{"height",			required_argument,	0,	'y'},
			{"is-server",		no_argument,				0,	's'},
			{"is-client",		no_argument,				0,	'c'},
			{"host",				required_argument,	0,	'h'},
			{"port",				required_argument,	0,	'p'},
			{"no-split", 		no_argument,				0,	'l'},
			{0, 0, 0, 0}
		};
		
		char option;
		int option_index = 0;
		
		while ((option = getopt_long(argc, argv, "fx:y:csh:p:l", long_options, &option_index)) != EOF)
		{
			switch (option) 
			{
				printf ("option %s", long_options[option_index].name);
				case 'f':
					fullscreen = true;
					break;
				case 'x':
					width = atoi(optarg);
					break;
				case 'y':
					height = atoi(optarg);
					break;
				case 'l':
					isSplit = false;
					break;
				case 'h':
					host = optarg;
					isSplit = false;
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
				case '?':
					usage();
					return 1;
			}
		}
		return 0;
	}
}

