#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "../header_files/Point_2d.h"
#include "../header_files/Render_Color.h"

class Bezier_Curve {
public:
	std::vector <std::array< Point_2d, 4>> points_2d;
	std::vector<Point_2d> processed_points_2d;

	Bezier_Curve();
	std::vector<Point_2d> calc_bezier_curve();
	void render_bezier_curve(SDL_Renderer* renderer, float x_pos, float y_pos,
		float scale, Render_Color color, SDL_FRect mouse_hitbox);
	void render_2d_control_points(SDL_Renderer* renderer, float x_pos, float y_pos, float scale);
	Point_2d check_if_control_point_in_range(SDL_FRect hitbox);
};