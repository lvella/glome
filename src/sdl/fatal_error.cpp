#include "fatal_error.hpp"

#include <iostream>
#include <SDL.h>

void fatal_user_error(const char* msg)
{
	std::cerr << msg << std::endl;
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					"Fatal Error!",
					msg,
					NULL);

	exit(1);
}
