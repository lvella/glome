#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#ifdef WIN32
#include <windows.h>
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <SDL.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "game.hpp"
#include "jsinput.hpp"

#include "native.hpp"

using namespace std;

gcn::Input* gcn_input()
{
	return new gcn::SDLInput();
}

gcn::ImageLoader* gcn_image_loader()
{
	return new gcn::OpenGLSDLImageLoader();
}

void list_video_modes(vector<string> &out)
{
	SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
	while(*modes) {
		char str[15];
		snprintf(str, 15, "%dx%d", (*modes)->w, (*modes)->h);
		out.push_back(string(str));
		++modes;
	}
}

static bool v_sync_enabled = true;

static void initialize_SDL()
{
	/* SDL Startup */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) != 0)
	{
		cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	{
		int video_flags = SDL_OPENGL | SDL_HWSURFACE | SDL_HWACCEL;
		if(Options::fullscreen)
			video_flags |= SDL_FULLSCREEN;
		SDL_SetVideoMode(Options::width, Options::height, 0, video_flags);
	}
	SDL_WM_SetCaption("Glome", NULL);
	SDL_ShowCursor(SDL_ENABLE);
	SDL_EnableUNICODE(1);
	SDL_JoystickEventState(SDL_ENABLE);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(SDL_DISABLE);
}

static void initialize_gl_context()
{
	// Using GLEW to get the OpenGL functions
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		cerr << "Unable to initialize GLEW:\n"
		<< glewGetErrorString(err) << endl;
		exit(1);
	}

	if(! GLEW_VERSION_2_1)
	{
		const char *msg = "Glome requires at least OpenGL 2.1";
		#ifdef WIN32
		MessageBoxA(NULL, msg, NULL, MB_OK);
		#else
		cerr << msg << endl;
		#endif
		exit(1);
	}

	// Enable V-Sync
	#ifdef WIN32
	if(WGLEW_EXT_swap_control)
		wglSwapIntervalEXT(1);
	#else
	if(GLXEW_SGI_swap_control)
		glXSwapIntervalSGI(1);
	#endif
	else
		v_sync_enabled = false;
}

static void main_loop()
{
	const int FPS = 60;
	uint64_t frame_count = 0;
	bool running = true;
	Uint32 start;
	Uint32 ticks;

	start = ticks = SDL_GetTicks();
	while(running) 
	{
		running = Input::handle();
		Game::frame();
		SDL_GL_SwapBuffers();
		// Fix framerate
		// TODO: deal with refresh rate different from 60 Hz
		if(!v_sync_enabled) {
			// TODO: maybe clk_div is useful here...
			const int period = 1000 / FPS;
			Uint32 now = SDL_GetTicks();
			int delay = period - int(now - ticks);
			if(delay > 0)
				SDL_Delay(delay);
			ticks = now;
		}
		++frame_count;
	}
	cout << frame_count << " frames rendered at " << frame_count / ((SDL_GetTicks() - start) / 1000.0) << " FPS.\n";
}

int main(int argc, char **argv)
{
	if (Options::parse_args(argc, argv))
		return 1;

	/* TODO: Network in game archive. */

	initialize_SDL();
	initialize_gl_context();
	Input::Js::initialize(0);
	Input::initialize();
	Game::initialize();

	main_loop();
}

