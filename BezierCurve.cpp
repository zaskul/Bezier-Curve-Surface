#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <fstream>
#include <string>
#include <regex>

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

struct point_2d {
    float x;
    float y;
};

struct point_3d {
    point_2d p_2d;
    float z;

    point_3d(float x, float y, float z) : p_2d{x, y}, z(z) {};

    point_3d() : p_2d{ 0, 0 }, z(0) {};
};

void draw_bezier_curve(std::vector<point_2d> xy, float px_density, float scale, SDL_Renderer* renderer) {
    
    for (float t = 0; t <= 1; t += px_density) {
        float px = pow(1 - t, 3) * pow(t, 0) * xy[0].x +
            3 * pow(1 - t, 2) * pow(t, 1) * xy[1].x +
            3 * pow(1 - t, 1) * pow(t, 2) * xy[2].x +
            pow(1 - t, 0) * pow(t, 3) * xy[3].x;
        float py = pow(1 - t, 3) * pow(t, 0) * xy[0].y +
            3 * pow(1 - t, 2) * pow(t, 1) * xy[1].y +
            3 * pow(1 - t, 1) * pow(t, 2) * xy[2].y +
            pow(1 - t, 0) * pow(t, 3) * xy[3].y;

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

void draw_bezier_surface(point_3d xyz[16], float px_density, float scale, SDL_Renderer* renderer) {
    for (float v = 0; v <= 1; v += px_density) {
        for (float w = 0; w <= 1; w += px_density) {
            float px = 0;
            float py = 0;
            float pz = 0;

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    px += xyz[i * 4 + j].p_2d.x * calc_bernstein(v, i) * calc_bernstein(w, j);
                    py += xyz[i * 4 + j].p_2d.y * calc_bernstein(v, i) * calc_bernstein(w, j);
                    pz += xyz[i * 4 + j].z * calc_bernstein(v, i) * calc_bernstein(w, j);
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

                    SDL_FRect scaled_pixel_yz = {
                        py * scale,
                        pz * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(renderer, &scaled_pixel_yz);
                }
                else {
                    SDL_RenderPoint(renderer, px, py);
                    SDL_RenderPoint(renderer, px, pz);
                    SDL_RenderPoint(renderer, py, pz);
                }
            }
            SDL_RenderPresent(renderer);
    }
}

std::vector<std::string> split_string(std::string str, std::string pattern) {
    std::regex regex_pattern(pattern);
    std::sregex_token_iterator iter(str.begin(), str.end(), regex_pattern, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> tokens(iter, end);
    return tokens;
}

int calc_matrix_size(std::vector<std::string> input) {
    int result = 1;
    while (!input.empty()) {
        std::string current_value = input.back();
        input.pop_back();
        result *= std::stoi(current_value) + 1;
    }
    return result;
}

std::vector<std::vector<point_3d>> read_file() {
    std::ifstream inputFile("Objects/teapotCGA.txt");
    if (!inputFile.is_open()) {
        std::cerr << "File could not be opened" << std::endl;
    }

    std::string line;
    std::getline(inputFile, line);
    int num_of_patches = stoi(line);
    
    std::vector<std::vector<point_3d>> points_3d(num_of_patches);

    for (int i = 0; i < points_3d.size(); i++) {
        std::getline(inputFile, line);
        int num_of_points = calc_matrix_size(split_string(line, "\\s+"));
        points_3d[i].resize(num_of_points);
        for (int j = 0; j < points_3d[i].size(); j++) {
            std::getline(inputFile, line);
            std::vector<std::string> temp_string_points = split_string(line, "\\s+");
            float temp_z = std::stof(temp_string_points.back());
            temp_string_points.pop_back();
            float temp_y = std::stof(temp_string_points.back());
            temp_string_points.pop_back();
            float temp_x = std::stof(temp_string_points.back());
            temp_string_points.pop_back();

            points_3d[i][j] = point_3d(temp_x, temp_y, temp_z);
        }
    }
    
    return points_3d;
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
    
    float px_density = 0.005;
    float scale = 30;

    std::vector<point_2d> points2d;
    points2d.push_back({ 1.0, 3.0 });
    points2d.push_back({ 2.0, 2.0 });
    points2d.push_back({ 3.0, 2.0 });
    points2d.push_back({ 4.0, 1.0 });

    std::vector<std::vector<point_3d>> points = read_file();

    // PRINT CURRENT SET OF POINTS
    std::cout << points.size() << std::endl;
    int iter = 0;
    while (!points.empty()) {
        std::vector<point_3d> temp_vec = points.back();
        std::cout << iter << std::endl;
        iter++;
        while (!temp_vec.empty()) {
            std::cout << "x: " << temp_vec.back().p_2d.x
                << " y: " << temp_vec.back().p_2d.y
                << " z: " << temp_vec.back().z <<
                std::endl;
            temp_vec.pop_back();
        }
        points.pop_back();
    }

    bool quit = false;
    //SDL_Event e;
    //while (!quit) {
    //    while (SDL_PollEvent(&e)) {
    //        if (e.type == SDL_EVENT_QUIT) {
    //            quit = true;
    //        }
    //        /*for (int i = 0; i < points.size(); i++) {
    //            draw_bezier_surface(points, 0.005, 20, renderer);
    //        }*/
    //        draw_bezier_curve(points2d, px_density, scale, renderer);
    //    }
    //}


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}