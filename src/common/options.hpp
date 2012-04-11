#pragma once

#include <string>

#ifdef NDEBUG
#define DEBUG_OPT const
#else
#define DEBUG_OPT
#endif

namespace Options
{

// Debug options
extern DEBUG_OPT int numBots;
extern DEBUG_OPT bool showBotScreen;

// Video options
extern const float FOV;
extern bool fullscreen;
extern int width;
extern int height;

// Network options
// TODO: Make use of these:
extern bool isServer;
extern bool isClient;
extern std::string host;
extern short port;

int parse_args(int argc,char** argv);
int load_config(std::string configfile);

}

