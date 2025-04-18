#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../header_files/Config.h"
#include "../header_files/Render_Color.h"
#include "../header_files/Bezier_Surface.h"
#include "../header_files/Bezier_Curve.h"
#include "../header_files/Perspective_Projection.h"
#include "../header_files/Text_Renderer.h"
#include "../header_files/Event_Handler.h"


int main()
{
    // init window and renderer
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Bezier curve", Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // init font
    if (TTF_Init() == false) {
        std::cerr << "SDL_ttf could not initialize!" << std::endl;
    }
    TTF_Font* font = TTF_OpenFont("ThirdParty/ARIAL.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font" << std::endl;
    }

    // init variables
    Render_Color color = { 255, 0, 0, 255 };
    Render_Color bg_color = { 0, 0, 0, 255 };
    Bezier_Surface bezier_surface = Bezier_Surface("teapotCGA.txt");
    Bezier_Curve bezier_curve = Bezier_Curve();
    Perspective_Projection projection = Perspective_Projection();
    Event_Handler event_handler = Event_Handler(bezier_surface, bezier_curve, projection, color);

    while (!event_handler.quit) {
        while (SDL_PollEvent(&event_handler.e)) {
            event_handler.handle_events();
        }
        // ONLY FOR 3D
        if (!event_handler.show_2d && event_handler.rotate_points) {
            projection.apply_projection(bezier_surface, event_handler.x_pos, event_handler.y_pos, event_handler.z_offset);
            event_handler.rotate_points = false;
        }
     
        // clear screen
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.b, bg_color.g, bg_color.a);
        SDL_RenderClear(renderer);

        if (!event_handler.show_2d) {
            // render surfaces
            bezier_surface.render_bezier_surfaces(renderer, event_handler.x_pos, event_handler.y_pos, color);
        }
        else {
            // render curves
            bezier_curve.render_bezier_curve(renderer, event_handler.x_pos, event_handler.y_pos,
                event_handler.point_scale, color, event_handler.mouse_hitbox);
        }
        if (event_handler.show_help_box) {
            Help_Box::render_help_box(renderer, font, event_handler.current_color, event_handler.mouse_wheel_action, event_handler.show_2d);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}