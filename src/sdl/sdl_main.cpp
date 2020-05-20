#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#ifdef _WIN32
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

static bool v_sync_enabled = true;

SDL_Window *window;
SDL_GLContext glcontext;

static void initialize_SDL()
{
	/* SDL Startup */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
	{
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_ShowCursor(SDL_ENABLE);
	SDL_JoystickEventState(SDL_ENABLE);

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

static void initialize_gl_context()
{
	//TODO: load anti-alias settings from configuration
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);*/

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

	// Using GLEW to get the OpenGL functions
	//glewExperimental = true;
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW:\n"
			<< glewGetErrorString(err) << std::endl;
		exit(1);
	}

	if(! GLEW_VERSION_3_2)
	{
		const char *msg = "Glome requires at least OpenGL 3.2";
		#ifdef WIN32
		MessageBoxA(NULL, msg, NULL, MB_OK);
		#else
		std::cerr << msg << std::endl;
		#endif
		exit(1);
	}

	int major, minor, mask;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);

	std::cout << "OpenGL version: " << major << '.' << minor;
	if(mask & GL_CONTEXT_CORE_PROFILE_BIT) {
		std::cout << " Core Profile";
	} else if(mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) {
		std::cout << " Compatibility Profile";
	}
	std::cout << std::endl;

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


	// Enable multisample, if available
	{
		int has_multisample;
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &has_multisample);

		int sample_size;
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &sample_size);

		if(has_multisample && sample_size > 1) {
			glEnable(GL_MULTISAMPLE);
		}
	}

	// Enable V-Sync
	if(SDL_GL_SetSwapInterval(-1) < 0 && SDL_GL_SetSwapInterval(1) < 0) {
		v_sync_enabled = false;
		std::cout << "V-Sync disabled" << std::endl;
	} else {
		std::cout << "V-Sync enabled" << std::endl;
	}
}

static void main_loop()
{
	using Timer = std::chrono::steady_clock;

	constexpr unsigned FIXED_FPS = 60;
	static_assert(FIXED_FPS > Game::MIN_FPS);

	uint64_t frame_count = 0;
	bool running = true;

	auto start = Timer::now();
	auto prev_time = start;
	while(running)
	{
		auto curr_time = Timer::now();
		running = Input::handle();
		Game::frame(curr_time - prev_time);
		SDL_GL_SwapWindow(window);

		// Fix framerate at FIXED_FPS
		if(!v_sync_enabled) {
			// TODO: maybe clk_div is useful here...
			constexpr std::chrono::duration<unsigned,
				std::ratio<1, FIXED_FPS>
			> period(1);

			auto after_frame = std::chrono::steady_clock::now();

			auto delay = period - (after_frame - curr_time);
			if(delay.count() > 0)
				std::this_thread::sleep_for(delay);
		}
		prev_time = curr_time;
		++frame_count;
	}

	std::chrono::duration<float> time_running = std::chrono::steady_clock::now() - start;
	std::cout << frame_count << " frames rendered at "
		<< frame_count / time_running.count() << " FPS.\n";
}

int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);

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

	return 0;
}
