#pragma once

#include <SDL/SDL.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <vector>

#include "guichan/opengl/openglsdlimageloader.hpp"
#include "guichan/opengl/openglgraphics.hpp"

using namespace std;

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
		int i;
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
		while(modes[++i]);
		nr_res = i;
	}

	int getNumberOfElements()
	{
		return nr_res;
	}

	string getElementAt(int i)
	{
		char str[10];
		sprintf(str, "%dx%d\0",modes[i]->w, modes[i]->h);
		return str;
	}
};


namespace Menu
{
	void menu_initialize();
	void menu_run();
	void menu_halt();
	void SDL_initialize();
}
















