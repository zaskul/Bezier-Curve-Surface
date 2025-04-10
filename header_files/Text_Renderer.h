#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
namespace Help_Box {
	void render_text(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, TTF_Font* font);
	void render_help_box(SDL_Renderer* renderer, TTF_Font* font, short current_color, short mouse_wheel_action, bool show_2d);
}