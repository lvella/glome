#pragma once

#include <string>

namespace Options
{
	extern const float FOV;
	extern int width;
	extern int height;
	extern bool fullscreen;
	extern std::string host;
	extern short port;

	extern bool isServer;
	extern bool isClient;
	extern bool isSplit;

	int parse_args(int argc,char** argv);
	int load_config(std::string configfile);
}

