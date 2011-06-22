#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "init_gl.hpp"
#include "menu.hpp"
#include "button.hpp"

#include "main.hpp"

using namespace std;

namespace Menu
{
	gcn::OpenGLSDLImageLoader* imageLoader;
	gcn::OpenGLGraphics* graphics;
	SDL_Surface* screen;
	SDL_Event event;
	
	gcn::Container* top;
	gcn::Gui* gui;
	gcn::ImageFont* font_normal;
	gcn::ImageFont* font_highlight;
	gcn::Label* label;
	bool running = true;
	extern gcn::Button* Singleplay_button;
	
	void menu_initialize()
	{
		/*
		 * SDL Startup
		 */
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0) 
		{
		  cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		  exit(1);
		}
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL /*| SDL_FULLSCREEN*/);
		SDL_WM_SetCaption("Navigna", NULL);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_EnableUNICODE(1);
		SDL_JoystickEventState(SDL_ENABLE);
		init_gl();
		
		imageLoader = new gcn::OpenGLSDLImageLoader();
    gcn::Image::setImageLoader(imageLoader);
    graphics = new gcn::OpenGLGraphics(WIDTH, HEIGHT);
    //graphics->setTarget(screen);
    
    /*
     * TODO: Generalize the paths to fonts in images/
     */
		font_normal = new gcn::ImageFont("test.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		gcn::Widget::setGlobalFont(font_normal);
		
		gui = new gcn::Gui();
    gui->setGraphics(graphics);
    
    top = new gcn::Container();    
    top->setDimension(gcn::Rectangle(0, 0, WIDTH, HEIGHT));
    gui->setTop(top);
    
    label = new gcn::Label("Navigna");
    label->setPosition(280, 120);
    top->add(label);
    /*
		* Init SinglePlayer Button
		* TODO: Take away that top stuff, make it global or something.
		*/
		Button::init_singleplay_button();
  }
  
  /*
   * TODO: Not sure if this input shuld be there, but eitherway
   * the input from the menu and the actual game are doing different
   * stuff... So I guess that's ok.
   */
  void menu_run()
	{
		bool running = true;
		gcn::SDLInput* input = new gcn::SDLInput();
		gui->setInput(input);
		while (running)
		{
			while(SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYDOWN)
				{
					if (event.key.keysym.sym == SDLK_ESCAPE)
						running = false;
				}
				else if(event.type == SDL_QUIT)
					running = false;
				input->pushInput(event);
			}
			gui->logic();
			gui->draw();
			SDL_GL_SwapBuffers();
			SDL_Flip(screen);
    }
	}
}
