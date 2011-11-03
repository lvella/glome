#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include <SDL/SDL.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>
#include <guichan/opengl/openglgraphics.hpp>

/*
 * This is a ListModel we will use for resultion.
 */
class ResolutionListModel : public gcn::ListModel
{
	
	SDL_Rect** modes;
	int nr_res;
	
public:
	void getModes()
	{
		int i = 0;
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
		while(modes[++i]);
		nr_res = i;
	}

	int getNumberOfElements()
	{
		return nr_res;
	}

	std::string getElementAt(int i)
	{
		char str[10];
		snprintf(str, 10, "%dx%d",modes[i]->w, modes[i]->h);
		return std::string(str);
	}
};


namespace Menu
{
	void menu_initialize();
	void menu_run();
	void menu_halt();
	void SDL_initialize();
}
















