#include "../header_files/Bezier_Surface.h"
#include "../header_files//Point_3d.h"
#include <iostream>
#include <fstream>
#include "../header_files/Utils.h"
#include "../header_files/Config.h"

Bezier_Surface::Bezier_Surface() {
    control_points_3d = read_file("teapotCGA.txt");
    prepare_3d_points(control_points_3d);
}

std::vector<Point_3d> Bezier_Surface::calc_bezier_surfaces(std::vector<Point_3d> points, float px_density) {
    std::vector<Point_3d> processed_points;
    for (float v = 0; v <= 1; v += px_density) {
        for (float w = 0; w <= 1; w += px_density) {
            Point_3d point = { 0, 0, 0 };
            std::vector<Point_3d>::iterator it = points.begin();
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    point.x += it->x * Utils::calc_bernstein(v, i) * Utils::calc_bernstein(w, j);
                    point.y += it->y * Utils::calc_bernstein(v, i) * Utils::calc_bernstein(w, j);
                    point.z += it->z * Utils::calc_bernstein(v, i) * Utils::calc_bernstein(w, j);
                    it++;
                }
            }
            processed_points.push_back(point);
        }
    }
    return processed_points;
}

void Bezier_Surface::render_bezier_surfaces(SDL_Renderer* renderer, float x_pos, float y_pos, Render_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (auto it = processed_points_3d.begin(); it != processed_points_3d.end(); it++) {
        SDL_RenderPoint(renderer, it->x, it->y);
    }
}

int Bezier_Surface::calc_matrix_size_from_file(std::vector<std::string> input) {
    int result = 1;
    while (!input.empty()) {
        std::string current_value = input.back();
        input.pop_back();
        result *= std::stoi(current_value) + 1;
    }
    return result;
}

std::vector<std::vector<Point_3d>> Bezier_Surface::read_file(std::string file_name) {
    std::string file_path = "Objects/" + file_name;
    std::ifstream inputFile(file_path);
    if (!inputFile.is_open()) {
        std::cerr << "File could not be opened" << std::endl;
    }

    std::string line;
    std::getline(inputFile, line);
    int num_of_patches = stoi(line);

    std::vector<std::vector<Point_3d>> points_3d(num_of_patches);

    for (int i = 0; i < points_3d.size(); i++) {
        std::getline(inputFile, line);
        int num_of_points = calc_matrix_size_from_file(Utils::split_string(line, "\\s+"));
        points_3d[i].resize(num_of_points);
        for (int j = 0; j < points_3d[i].size(); j++) {
            std::getline(inputFile, line);
            std::vector<std::string> temp_string_points = Utils::split_string(line, "\\s+");
            float temp_z = std::stof(temp_string_points.back());
            temp_string_points.pop_back();
            float temp_y = std::stof(temp_string_points.back());
            temp_string_points.pop_back();
            float temp_x = std::stof(temp_string_points.back());
            temp_string_points.pop_back();

            points_3d[i][j] = Point_3d(temp_x, temp_y, temp_z);
        }
    }
    inputFile.close();
    return points_3d;
}

void Bezier_Surface::prepare_3d_points(std::vector<std::vector<Point_3d>>& pts) {
    std::vector<Point_3d> temp_out_vec;
    for (auto it = pts.begin(); it != pts.end(); it++) {
        std::vector<Point_3d> temp_vec = calc_bezier_surfaces(*it, Config::PX_DENSITY);
        temp_out_vec.reserve(temp_out_vec.size() + temp_vec.size());
        temp_out_vec.insert(temp_out_vec.end(), temp_vec.begin(), temp_vec.end());
    }
    processed_points_3d = temp_out_vec;
}

void Bezier_Surface::scale_into_view(float x_pos, float y_pos) {
    for (auto it = projected_points.begin(); it != projected_points.end(); it++) {
        it->x = (it->x + x_pos) * 0.5f * (float)Config::WINDOW_WIDTH / 2;
        it->y = (it->y + y_pos) * 0.5f * (float)Config::WINDOW_WIDTH / 2;
    }
}

void Bezier_Surface::translate_points(std::vector<Point_3d>& pts, float z_offset) {
    for (auto it = pts.begin(); it != pts.end(); it++) {
        it->z += z_offset;
    }
}