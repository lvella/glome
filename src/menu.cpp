#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "menu.hpp"
#include "button.hpp"
#include "game.hpp"
#include "init_gl.hpp"
#include "options.hpp"

using namespace std;
using namespace Options;

namespace Menu
{
	gcn::Container* top;
	gcn::Container* mainC;
	gcn::Container* optionsC;
	
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
			else if (actionEvent.getId() == "options")
			{
				mainC->setVisible(false);
				optionsC->setVisible(true);
			}
			else if (actionEvent.getId() == "back_main")
			{
				mainC->setVisible(true);
				optionsC->setVisible(false);
			}
		}
	};
	
	gcn::OpenGLSDLImageLoader* imageLoader;
	gcn::OpenGLGraphics* graphics;
	SDL_Surface* screen;
	SDL_Event event;
	gcn::Gui* gui;
	gcn::ImageFont* font_normal;
	gcn::ImageFont* font_highlight;
	gcn::Label* l_main;
	bool running = true;
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
    screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_HWACCEL /*SDL_FULLSCREEN*/);
    SDL_WM_SetCaption("Navigna", NULL);
		SDL_ShowCursor(SDL_ENABLE);
		SDL_EnableUNICODE(1);
		SDL_JoystickEventState(SDL_ENABLE);
		
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);	
		imageLoader = new gcn::OpenGLSDLImageLoader();
		gcn::Image::setImageLoader(imageLoader);
		graphics = new gcn::OpenGLGraphics(width, height);
		graphics->setTargetPlane(width, height);
		font_normal = new gcn::ImageFont("normal.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		font_highlight = new gcn::ImageFont("highlight.png", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		
		gcn::Widget::setGlobalFont(font_normal);
		
		input = new gcn::SDLInput();
		gui = new gcn::Gui();
		gui->setGraphics(graphics);
		gui->setInput(input);
		
		top = new gcn::Container();
		mainC = new gcn::Container();
		optionsC = new gcn::Container();
		
		mainC->setVisible(true);
		optionsC->setVisible(false);
		top->add(mainC);
		top->add(optionsC);
		
		top->setDimension(gcn::Rectangle(0, 0, width, height));
		mainC->setDimension(gcn::Rectangle(0, 0, width, height));
		optionsC->setDimension(gcn::Rectangle(0, 0, width, height));
		
		gui->setTop(top);
		l_main = new gcn::Label("Navigna");
    l_main->setPosition(width/2-100, height/2-300);
    mainC->add(l_main);
    
    gcn::Label* l_options = new gcn::Label("Options");
    l_options->setPosition(width/2-100, height/2-300);
    optionsC->add(l_options);
    
    ButtonActionListener* buttonActionListener = new ButtonActionListener();
    /*
     * Init Main Buttons
    */
    //Single Player Button
    NButton* singleplay_button = singleplay_button = new NButton("Singleplayer");
		singleplay_button->setHL_font(font_highlight);
		singleplay_button->setActionEventId("singleplay");
		singleplay_button->addActionListener(buttonActionListener);
		Menu::mainC->add(singleplay_button);
		singleplay_button->setPosition(width/2 - 100, height/2 - 100);
		
		//Options Button
		NButton* options_button = options_button = new NButton("Options");
		options_button->setHL_font(font_highlight);
		options_button->setActionEventId("options");
		options_button->addActionListener(buttonActionListener);
		Menu::mainC->add(options_button);	
		options_button->setPosition(width/2 - 60, height/2 - 30);
		
		/*
     * Init Options Buttons
    */
    
    
    //Back Button
		NButton* back_main = singleplay_button = new NButton("Back");
		back_main->setHL_font(font_highlight);
		back_main->setActionEventId("back_main");
		back_main->addActionListener(buttonActionListener);
		Menu::optionsC->add(singleplay_button);
		back_main->setPosition(width/2 - 60, height/2 - 100);
		
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
