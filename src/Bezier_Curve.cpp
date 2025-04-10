#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../header_files/Point_2d.h"
#include "../header_files/Utils.h"
#include "../header_files/Config.h"
#include "../header_files/Bezier_Curve.h"
#include "../header_files/Render_Color.h"

Bezier_Curve::Bezier_Curve() {
    // pre-made initial letter
    points_2d = {
    { {{ 174.f, 59.f }, { 146.f, 10.f },{ 131.f, 76.f }, { 131.f, 86.f }} },
    { {{ 131.f, 86.f }, { 131.f, 96.f }, { 124.f, 195.f }, { 119.f, 209.f }} },
    { {{ 119.f, 209.f }, { 110.f, 227.f }, { 84.f, 228.f }, { 83.f, 217.f }} },
    { {{ 83.f, 217.f }, { 79.f, 180.f}, { 136.f, 215.f }, { 142.f, 223.f }} },
    { {{ 142.f, 223.f }, { 154.f, 239.f }, { 195.f, 204.f}, { 193.f, 197.f }} }
    };

    // add the crossing line
    float x_offset = 30.f;
    float y_offset = 60.f;
    points_2d.push_back({
        {{(88.f + x_offset) / 2.f, (193.f + y_offset) / 2.f},
         {(294.f + x_offset) / 2.f, (113.f + y_offset) / 2.f},
         {(189.f - x_offset) / 2.f, (282.f + y_offset) / 2.f},
         {(394.f - x_offset) / 2.f, (188.f + y_offset) / 2.f}}
        });

    // move the letter to center of the screen
    x_offset = (float)Config::WINDOW_WIDTH / 3;
    y_offset = (float)Config::WINDOW_HEIGHT / 3;
    for (auto it = points_2d.begin(); it != points_2d.end(); it++) {
        for (auto inner_it = it->begin(); inner_it != it->end(); inner_it++) {
            inner_it->x += x_offset;
            inner_it->y += y_offset;
        }
    }

    calc_bezier_curve();

}

std::vector<Point_2d> Bezier_Curve::calc_bezier_curve() {
    std::vector<Point_2d> res_vector;
    Point_2d point = Point_2d();
    for (auto vec_it = points_2d.begin(); vec_it != points_2d.end(); vec_it++) {
        // read individual curve
        float x[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float y[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        int i = 0;
        std::array<Point_2d, 4>::iterator check_dist = vec_it->begin();
        if (std::distance(check_dist, vec_it->end()) < 4) return res_vector;
        for (auto it = vec_it->begin(); it != vec_it->end(); it++) {
            x[i] = it->x;
            y[i] = it->y;
            i++;
        }
        // calculate the curve
        for (float t = 0; t <= 1.0f; t += Config::PX_DENSITY) {
            point.x = Utils::calc_bernstein(t, 0) * x[0] +
                Utils::calc_bernstein(t, 1) * x[1] +
                Utils::calc_bernstein(t, 2) * x[2] +
                Utils::calc_bernstein(t, 3) * x[3];
            point.y = Utils::calc_bernstein(t, 0) * y[0] +
                Utils::calc_bernstein(t, 1) * y[1] +
                Utils::calc_bernstein(t, 2) * y[2] +
                Utils::calc_bernstein(t, 3) * y[3];
            res_vector.push_back(point);
        }
    }
    processed_points_2d = res_vector;
}

void Bezier_Curve::render_bezier_curve(SDL_Renderer* renderer, float x_pos, float y_pos, float scale, Render_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (auto it = processed_points_2d.begin(); it != processed_points_2d.end(); it++) {
        //SDL_SetRenderScale(renderer, scale, scale);
        SDL_RenderPoint(renderer, it->x + x_pos, it->y + y_pos);
        //SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    }
}

void Bezier_Curve::render_2d_control_points(SDL_Renderer* renderer, float x_pos, float y_pos, float scale) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (auto it = points_2d.begin(); it != points_2d.end(); it++) {
        for (auto array_it = it->begin(); array_it != it->end(); array_it++) {
            //SDL_SetRenderScale(renderer, scale, scale);
            SDL_RenderPoint(renderer, array_it->x + x_pos, array_it->y + y_pos);
            //SDL_SetRenderScale(renderer, 1.0f, 1.0f);
        }
    }
}

Point_2d Bezier_Curve::check_if_control_point_in_range(SDL_FRect hitbox) {
    std::vector<Point_2d> candidates = {};
    std::vector<Point_2d> pos_vector = {};
    float row = 0.0f;
    float col = 0.0f;
    for (auto it = points_2d.begin(); it != points_2d.end(); it++) {
        for (auto array_it = it->begin(); array_it != it->end(); array_it++) {
            SDL_FPoint sdl_point = { array_it->x, array_it->y };
            if (SDL_PointInRectFloat(&sdl_point, &hitbox)) {
                Point_2d pos = { row, col };
                pos_vector.push_back(pos);
            }
            col++;
        }
        row++;
        col = 0.0f;
    }
    if (pos_vector.size() != 0) {
        return *pos_vector.begin();
    }
    else {
        return Point_2d(-1, -1);
    }
}