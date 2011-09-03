#pragma once

#include <guichan.hpp>
#include <guichan/sdl.hpp>

#include "guichan/opengl/openglsdlimageloader.hpp"
#include "guichan/opengl/openglgraphics.hpp"

using namespace std;

/*
 * This is a ListModel we will use for resultion.
 */
class ResolutionListModel : public gcn::ListModel
{
public:
	int getNumberOfElements()
	{
		return 2;
	}

	std::string getElementAt(int i)
	{
		switch(i)
		{
			case 0:
				return std::string("1024x768");
			case 1:
				return std::string("800x600");
			default:
				return std::string("");
		}
	}
};


namespace Menu
{
	extern gcn::Container* top;
	void menu_initialize();
	void menu_run();
	void menu_halt();
}
















