#include <iostream>
#include <SDL3/SDL.h>

void draw_bezier_curve(int x[4], int y[4], float px_density, float scale, SDL_Renderer* renderer) {
    
    for (float t = 0; t < 1; t += px_density) {
        float px = pow(1 - t, 3) * pow(t, 0) * x[0] +
            3 * pow(1 - t, 2) * pow(t, 1) * x[1] +
            3 * pow(1 - t, 1) * pow(t, 2) * x[2] +
            pow(1 - t, 0) * pow(t, 3) * x[3];
        float py = pow(1 - t, 3) * pow(t, 0) * y[0] +
            3 * pow(1 - t, 2) * pow(t, 1) * y[1] +
            3 * pow(1 - t, 1) * pow(t, 2) * y[2] +
            pow(1 - t, 0) * pow(t, 3) * y[3];
        printf("x: %f, y: %f \n", px, py);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        if (scale != 0) {
            SDL_FRect scaled_pixel = {
                px * scale,
                py * scale,
                scale,
                scale
            };
            SDL_RenderFillRect(renderer, &scaled_pixel);
        }
        else {
            SDL_RenderPoint(renderer, px, py);
        }
    }
    SDL_RenderPresent(renderer);
}

int main()
{
    int width = 320;
    int height = 180;
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Bezier curve", width, height, 0);
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

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    int x_pos[4] = { 1, 2, 3, 4 };
    int y_pos[4] = { 3, 2, 2, 1 };
    float px_density = 0.005;
    float scale = 30;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        draw_bezier_curve(x_pos, y_pos, px_density, scale, renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}