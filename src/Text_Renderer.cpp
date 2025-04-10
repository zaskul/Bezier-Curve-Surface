#include "../header_files/Text_Renderer.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

// render keybindings help box
namespace Help_Box {
    void Help_Box::render_text(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, TTF_Font* font) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int text_width = surface->w;
        int text_height = surface->h;
        SDL_DestroySurface(surface);

        SDL_FRect dest_rect = { (float)x, (float)y, (float)text_width, (float)text_height };
        SDL_RenderTexture(renderer, texture, NULL, &dest_rect);

        SDL_DestroyTexture(texture);
    }
    void Help_Box::render_help_box(SDL_Renderer* renderer, TTF_Font* font, short current_color, short mouse_wheel_action, bool show_2d) {
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Color variable_color = { 0, 255, 0 , 255 };
        std::string current_color_array[4] = { "Red", "Green", "Blue", "Alpha" };
        std::string mouse_wheel_action_array[2] = { "Color", "Zoom" };
        int x = 20, y = 20;  // Position of the help box
        render_text(renderer, "CONTROLS:", x, y, white, font);
        y += 30;
        render_text(renderer, "W / S  - Move Up/Down", x, y, white, font);
        y += 25;
        render_text(renderer, "A / D  - Move Left/Right", x, y, white, font);
        y += 25;
        if (!show_2d) {
            render_text(renderer, "+ / -  - Offset object in the Z axis", x, y, white, font);
            y += 25;
            render_text(renderer, "M / N  - Change color / Zoom on scroll", x, y, white, font);
            y += 25;
            render_text(renderer, "Up / Down arrow - Rotate X Axis", x, y, white, font);
            y += 25;
            render_text(renderer, "Left / Right arrow - Rotate Y Axis", x, y, white, font);
            y += 25;
            render_text(renderer, "R - Reset Transformations", x, y, white, font);
            y += 25;
        }
        else {
            render_text(renderer, "R - Reset movement and scale", x, y, white, font);
            y += 25;
            render_text(renderer, "T - Remove most recent curve", x, y, white, font);
            y += 25;
            render_text(renderer, "M / N  - Change color / scale on scroll", x, y, white, font);
            y += 25;
            render_text(renderer, "0 - Move/Add control points", x, y, white, font);
            y += 25;
        }
        render_text(renderer, "V / B Select color to change R>G>B", x, y, white, font);
        y += 25;
        std::string col = "Current color: " + current_color_array[current_color];
        render_text(renderer, col, x, y, variable_color, font);
        y += 25;
        std::string mwa = "Current action: Change " + mouse_wheel_action_array[mouse_wheel_action];
        render_text(renderer, mwa, x, y, variable_color, font);
        y += 25;
        render_text(renderer, "H - Show/Hide this box", x, y, white, font);
    }
}
