#include <iostream>
#include <sstream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "menu.hpp"
#include "button.hpp"
#include "game.hpp"
#include "init_gl.hpp"
#include "options.hpp"
#include "config.hpp"

using namespace std;
using namespace Options;

namespace Menu
{
	gcn::Container* top;
	gcn::Container* mainC;
	gcn::Container* optionsC;
	bool res_changed = false;
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
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
			else if (actionEvent.getId() == "resolution")
			{
				res_changed = true;
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
	gcn::ImageFont* font_menu;
	gcn::Label* l_main;
	bool running = true;
	gcn::SDLInput* input = new gcn::SDLInput();
	
	gcn::DropDown* resolution;
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

		std::stringstream normal,highlight,menufont;
		normal << DATA_DIR << "/normal.png";
		highlight << DATA_DIR << "/highlight.png";
		menufont << DATA_DIR << "/menufont.png";

		font_normal = new gcn::ImageFont(normal.str().c_str(), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		font_highlight = new gcn::ImageFont(highlight.str().c_str(), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		font_menu = new gcn::ImageFont(menufont.str().c_str(), " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
		gcn::Widget::setGlobalFont(font_menu);
		
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
		
		/*
		 * Labels
		*/
		l_main = new gcn::Label("Navigna");
    l_main->setPosition(width/2-100, height/2-300);
    l_main->setFont(font_normal);
    mainC->add(l_main);
    l_main->adjustSize();
    
    gcn::Label* l_options = new gcn::Label("Options");
    l_options->setPosition(width/2-100, height/2-300);
    l_options->setFont(font_normal);
    optionsC->add(l_options);
    l_options->adjustSize();
    
    gcn::Label* l_resolution = new gcn::Label("Resolution");
    l_resolution->setPosition(width/2 -200, height/2 -70);
    optionsC->add(l_resolution);
    
    ButtonActionListener* buttonActionListener = new ButtonActionListener();
    /*
     * Init Main Buttons
    */
    //Single Player Button
    NButton* singleplay_button = singleplay_button = new NButton("Singleplayer");
		singleplay_button->setHL_font(font_highlight);
		singleplay_button->setActionEventId("singleplay");
		singleplay_button->addActionListener(buttonActionListener);
		singleplay_button->setPosition(width/2 - 100, height/2 - 100);
		singleplay_button->setFont(font_normal);
		Menu::mainC->add(singleplay_button);
		singleplay_button->adjustSize();
		
		//Options Button
		NButton* options_button = options_button = new NButton("Options");
		options_button->setHL_font(font_highlight);
		options_button->setActionEventId("options");
		options_button->addActionListener(buttonActionListener);
		options_button->setPosition(width/2 - 60, height/2 - 30);
		options_button->setFont(font_normal);
		Menu::mainC->add(options_button);	
		options_button->adjustSize();
		
		/*
     * Init Options Buttons
    */
    //CheckList
		ResolutionListModel* res_model = new ResolutionListModel();
		gcn::ScrollArea* scroll_area = new gcn::ScrollArea();
		scroll_area->setBackgroundColor(0x32f000);
		scroll_area->setForegroundColor(0x32f000);
		scroll_area->setSelectionColor(0x552020);
		
		gcn::ListBox* list_box = new gcn::ListBox();
		list_box->setBackgroundColor(0x32f000);
		list_box->setForegroundColor(0x32f000);
		list_box->setSelectionColor(0x552020);
		
		list_box->adjustSize();
    resolution = new gcn::DropDown(res_model,
									scroll_area,
									list_box);
		resolution->setBackgroundColor(0x32f000);
		resolution->setForegroundColor(0x32f000);
		resolution->setSelectionColor(0x552020);
    resolution->setWidth(200);
    resolution->addActionListener(buttonActionListener);
    resolution->setActionEventId("resolution");
    resolution->setPosition(width/2 - 90, height/2 -70);
    Menu::optionsC->add(resolution);

    //Back Button
		NButton* back_main = singleplay_button = new NButton("Back");
		back_main->setHL_font(font_highlight);
		back_main->setActionEventId("back_main");
		back_main->addActionListener(buttonActionListener);
		back_main->setPosition(width/2 - 60, height/2 + 150);
		back_main->setFont(font_normal);
		Menu::optionsC->add(back_main);
		back_main->adjustSize();
		//TODO: Change to pick from configure
		resolution->setSelected(3);
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
			
			if (res_changed)
			{cout << resolution->getSelected() << endl;
				width = 1280;
				height = 728;
				screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_HWACCEL /*SDL_FULLSCREEN*/);
				res_changed = false;
			}
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

	void menu_halt()
	{
		delete top;
		delete mainC;
		delete optionsC;
		delete imageLoader;
		delete graphics;
		delete gui;
		delete font_normal;
		delete font_highlight;
		delete l_main;
		delete input;
		SDL_Quit();
	}
}
