#pragma once
#include <vector>
#include <array>
#include "Point_3d.h"
#include <SDL3/SDL.h>
#include "Render_Color.h"
#include <string>

class Bezier_Surface {
public:
	Bezier_Surface();
	// init point vectors
	std::vector<std::vector<Point_3d>> control_points_3d;
	std::vector<Point_3d> processed_points_3d, rotated_points, projected_points;

	std::vector<Point_3d> calc_bezier_surfaces(std::vector<Point_3d> points, float px_density);
	void render_bezier_surfaces(SDL_Renderer* renderer, float x_pos, float y_pos, Render_Color color);
	std::vector<std::vector<Point_3d>> read_file(std::string file_name);
	void prepare_3d_points(std::vector<std::vector<Point_3d>>& pts);
	void translate_points(std::vector<Point_3d>& pts, float z_offset);
	int calc_matrix_size_from_file(std::vector<std::string> input);
	void scale_into_view(float x_pos, float y_pos);
};