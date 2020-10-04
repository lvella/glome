#include <popup_window.hpp>

void fatal_user_error(const char* title, const char* msg) {

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					title,
					msg,
					NULL);
}