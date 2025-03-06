#include <iostream>
#include <SDL3/SDL.h>

void draw_bezier_curve(int x[4], int y[4], float px_density, float scale, SDL_Renderer* renderer) {
    
    for (float t = 0; t <= 1; t += px_density) {
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


float calc_bernstein(float vw, int ij) {
    switch (ij)
    {
    case 0:
        return pow(1 - vw, 3) * pow(vw, 0);
        break;
    case 1:
        return 3 * pow(1 - vw, 2) * pow(vw, 1);
        break;
    case 2:
        return 3 * pow(1 - vw, 1) * pow(vw, 2);
        break;
    case 3:
        return pow(1 - vw, 0) * pow(vw, 3);
        break;
    default:
        break;
    }
}

void draw_bezier_surface(float xyz[16][3], float px_density, float scale, SDL_Renderer* renderer) {
    for (float v = 0; v <= 1; v += px_density) {
        for (float w = 0; w <= 1; w += px_density) {
            float px = 0;
            float py = 0;
            float pz = 0;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    px += xyz[i * 4 + j][0] * calc_bernstein(v, i) * calc_bernstein(w, j);
                    py += xyz[i * 4 + j][1] * calc_bernstein(v, i) * calc_bernstein(w, j);
                    pz += xyz[i * 4 + j][2] * calc_bernstein(v, i) * calc_bernstein(w, j);
                }
            }
            //printf("px: %f, py: %f, pz: %f \n", px, py, pz);

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                if (scale != 0) {
                    SDL_FRect scaled_pixel_xy = {
                        px * scale,
                        py * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(renderer, &scaled_pixel_xy);

                    SDL_FRect scaled_pixel_xz = {
                        px * scale,
                        pz * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(renderer, &scaled_pixel_xz);

                    SDL_FRect scaled_pixel_zy = {
                        pz * scale,
                        py * scale,
                        scale,
                        scale
                    };
                    //SDL_RenderFillRect(renderer, &scaled_pixel_zy);
                }
                else {
                    SDL_RenderPoint(renderer, px, py);
                }
            }
            SDL_RenderPresent(renderer);
    }
}

int main()
{
    int width = 640;
    int height = 360;
    
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

    float temp[16][3] = {
        {1.4, 0.0, 2.4},
        {1.4, -0.784, 2.4},
        {0.784, -1.4, 2.4},
        {0.0, -1.4, 2.4},
        {1.3375, 0.0, 2.53125},
        {1.3375, -0.749, 2.53125},
        {0.749, -1.3375, 2.53125},
        {0.0, -1.3375, 2.53125},
        {1.4375, 0.0, 2.53125},
        {1.4375, -0.805, 2.53125},
        {0.805, -1.4375, 2.53125},
        {0.0, -1.4375, 2.53125},
        {1.5, 0.0, 2.4},
        {1.5, -0.84, 2.4},
        {0.84, -1.5, 2.4},
        {0.0, -1.5, 2.4}
    };

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            draw_bezier_surface(temp, 0.005, 40, renderer);
        }
        //draw_bezier_curve(x_pos, y_pos, px_density, scale, renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}