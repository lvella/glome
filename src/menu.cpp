#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "menu.hpp"
#include "button.hpp"
#include "game.hpp"
#include "init_gl.hpp"
#include "main.hpp"

using namespace std;

class ButtonActionListener : public gcn::ActionListener
	{
		public:
		void action(const gcn::ActionEvent& actionEvent)
		{
			if (actionEvent.getId() == "singleplay")
			{
				init_gl();
				Game::init_game();
				Game::main_loop();
			}
		}
	};

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
	gcn::SDLInput* input = new gcn::SDLInput();
	
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
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_HWACCEL /*SDL_FULLSCREEN*/);
    SDL_WM_SetCaption("Navigna", NULL);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_EnableUNICODE(1);
		SDL_JoystickEventState(SDL_ENABLE);
		
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);	
		imageLoader = new gcn::OpenGLSDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);
		graphics = new gcn::OpenGLGraphics(WIDTH, HEIGHT);
		graphics->setTargetPlane(WIDTH, HEIGHT);
		font_normal = new gcn::ImageFont("normal.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		font_highlight = new gcn::ImageFont("highlight.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		
		gcn::Widget::setGlobalFont(font_normal);
		
		input = new gcn::SDLInput();
		gui = new gcn::Gui();
		gui->setGraphics(graphics);
		gui->setInput(input);
		top = new gcn::Container();
		top->setDimension(gcn::Rectangle(0, 0, WIDTH, HEIGHT));
		gui->setTop(top);
		label = new gcn::Label("Navigna");
    label->setPosition(WIDTH/2-100, HEIGHT/2-300);
    top->add(label);
    
    ButtonActionListener* buttonActionListener = new ButtonActionListener();
    /*
     * Init Buttons
    */
    NButton* singleplay_button = singleplay_button = new NButton("Singleplayer");
		singleplay_button->setHL_font(font_highlight);
		singleplay_button->setActionEventId("singleplay");
		singleplay_button->addActionListener(buttonActionListener);
		
		Menu::top->add(singleplay_button);
		
		
		singleplay_button->setPosition(WIDTH/2 - 100, HEIGHT/2 - 100);
		//singleplay_button->adjustSize();
    
    
	}
  
  /*
   * TODO: Not sure if this input shuld be there, but eitherway
   * the input from the menu and the actual game are doing different
   * stuff... So I guess that's ok.
   */
  void menu_run()
	{
		bool running = true;
		//gui->setInput(input);
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
