#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#ifdef WIN32
#include <windows.h>
#include <GL/wglew.h>
#else
#include <GL/glxew.h>
#endif

#include <SDL.h>

#include "input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "game.hpp"
#include "jsinput.hpp"

#include "native.hpp"

using namespace std;

static bool v_sync_enabled = true;

SDL_Window *window;
SDL_GLContext glcontext;

static void initialize_SDL()
{
	/* SDL Startup */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
	{
		cerr << "Unable to initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	{
		Uint32 video_flags = SDL_WINDOW_OPENGL;
		if(Options::fullscreen)
			video_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

		window = SDL_CreateWindow(
	        "Glome",
	        SDL_WINDOWPOS_UNDEFINED,
    	    SDL_WINDOWPOS_UNDEFINED,
			Options::width,
			Options::height,
			video_flags);
	}
	glcontext = SDL_GL_CreateContext(window);

	SDL_ShowCursor(SDL_ENABLE);
	SDL_JoystickEventState(SDL_ENABLE);

	SDL_SetRelativeMouseMode(SDL_TRUE);
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
	if(SDL_GL_SetSwapInterval(-1) < 0 && SDL_GL_SetSwapInterval(1) < 0) {
		v_sync_enabled = false;
		cout << "V-Sync disabled" << endl;
	} else {
		cout << "V-Sync enabled" << endl;
	}
}

static void main_loop()
{
	typedef std::chrono::steady_clock Timer;
	typedef std::chrono::duration<float> FloatSec;

	const int FPS = 60;
	uint64_t frame_count = 0;
	bool running = true;

	auto start = std::chrono::steady_clock::now();
	auto ticks = start;
	while(running) 
	{
		running = Input::handle();
		Game::frame();
		SDL_GL_SwapWindow(window);
		// Fix framerate
		// TODO: deal with refresh rate different from 60 Hz
		if(!v_sync_enabled) {
			// TODO: maybe clk_div is useful here...
			const std::chrono::duration<unsigned, std::ratio<1, FPS>> period(1);
			auto now = Timer::now();

			auto delay = period - (now - ticks);
			if(delay.count() > 0)
				std::this_thread::sleep_for(delay);

			ticks = now;
		}
		++frame_count;
	}

	FloatSec time_running = Timer::now() - start;
	cout << frame_count << " frames rendered at " << frame_count / time_running.count() << " FPS.\n";
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

	Game::shutdown();
}

