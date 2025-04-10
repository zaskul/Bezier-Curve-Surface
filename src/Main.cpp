#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <regex>
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../header_files/Utils.h"
#include "../header_files/Config.h"
#include "../header_files/Point_2d.h"
#include "../header_files/Point_3d.h"
#include "../header_files/Render_Color.h"
#include "../header_files/Bezier_Surface.h"
#include "../header_files/Bezier_Curve.h"
#include "../header_files/Matrices.h"
#include "../header_files/Perspective_Projection.h"
#include "../header_files/Text_Renderer.h"
#define _USE_MATH_DEFINES


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

    Bezier_Surface bezier_surface = Bezier_Surface();
    Bezier_Curve bezier_curve = Bezier_Curve();
    Perspective_Projection projection = Perspective_Projection();
    

    // init variables
    bool quit = false;
    SDL_Event e;
    float rotation_angle = 10.0f;
    float x_pos = 1.0f;
    float y_pos = 1.0f;
    float movement_step = 0.05f;
    float z_offset = 6.0f;
    int color_step = 15;
    short current_color = 0;
    short mouse_wheel_action = 0;
    Render_Color color = { 255, 0, 0, 255 };
    Render_Color bg_color = { 0, 0, 0, 255 };
    bool show_help_box = true;
    float point_scale = 1.0f;

    bool show_2d = false;

    // 3D rotation engaged
    bool rotate_points = true;

    // 2D curve move/add
    bool add_new_points = true;
    std::array<Point_2d, 4> new_curve = {};
    size_t points_in_array = 0;
    bool is_clicked = false;
    SDL_FRect mouse_hitbox = {0.0f, 0.0f, 0.0f, 0.0f};
    float mouse_hitbox_size = 10.0f;
    bool move_points = false;
    Point_2d point_selected;
    bool change_point_pos = true;
    bool point_moved = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (show_2d) {
                    // move control point
                    mouse_hitbox = { e.button.x - mouse_hitbox_size / 2 - x_pos, e.button.y - mouse_hitbox_size / 2 - y_pos, mouse_hitbox_size, mouse_hitbox_size };
                    if (move_points) {
                        point_selected = bezier_curve.check_if_control_point_in_range(mouse_hitbox);
                        std::cout << "x :" << point_selected.x << " y: " << point_selected.y << std::endl;
                        if (point_selected.x != -1.0f && point_selected.y != -1.0f && !point_moved) {
                            change_point_pos = true;
                            move_points = false;
                        }
                    }
                    else if (change_point_pos && !add_new_points) {
                        std::vector<std::array<Point_2d, 4>>::iterator iter = bezier_curve.points_2d.begin() + point_selected.x;
                        std::array<Point_2d, 4>::iterator iter2 = iter->begin() + point_selected.y;
                        iter2->x = e.button.x;
                        iter2->y = e.button.y;
                        point_moved = true;
                        change_point_pos = false;
                    }
                    // add new curve
                    else if (points_in_array < 2 && !is_clicked && add_new_points) {
                        std::cout << "ADDED x:" << e.button.x << "y: " << e.button.y << std::endl;
                        // add to pos 0 then to pos 3
                        new_curve[points_in_array * 3] = Point_2d(e.button.x - x_pos, e.button.y - y_pos);
                        points_in_array++;
                        is_clicked = true;
                    }
                    // offset the hitbox to display correctly
                    mouse_hitbox.x += x_pos;
                    mouse_hitbox.y += y_pos;
                }
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (show_2d) {
                    is_clicked = false;
                    // create a straight line
                    if (!change_point_pos && !add_new_points) {
                        bezier_curve.calc_bezier_curve();
                        move_points = true;
                        point_moved = false;
                    }
                    else if (points_in_array == 2 && add_new_points) {
                        Point_2d P0 = new_curve[0];
                        Point_2d P3 = new_curve[3];
                        // create point at 1/3 and 2/3 of the line length
                        Point_2d P1 = {
                            P0.x + (P3.x - P0.x) / 3.0f,
                            P0.y + (P3.y - P0.y) / 3.0f
                        };
                        Point_2d P2 = {
                            P0.x + (2.0f * (P3.x - P0.x)) / 3.0f,
                            P0.y + (2.0f * (P3.y - P0.y)) / 3.0f
                        };
                        new_curve[1] = P1;
                        new_curve[2] = P2;
                        bezier_curve.points_2d.push_back(new_curve);
                        bezier_curve.calc_bezier_curve();
                        points_in_array = 0;
                    }
                }
            }
            if (e.type == SDL_EVENT_MOUSE_WHEEL) {
                if (e.wheel.y > 0) {
                    std::cout << "UP" << std::endl;
                    if (mouse_wheel_action == 0) {
                        color.change_color(color, current_color, color_step);
                    }
                    else if (mouse_wheel_action == 1) {
                        if (!show_2d) {
                            z_offset += 1.f;
                            rotate_points = true;
                        }
                        else {
                            point_scale++;
                        }
                    }
                }
                if (e.wheel.y < 0) {
                    std::cout << "DOWN" << std::endl;
                    if (mouse_wheel_action == 0) {
                        color.change_color(color, current_color, -color_step);
                    }
                    else if (mouse_wheel_action == 1) {
                        if (!show_2d) {
                            if (z_offset != 0.0f) {
                                z_offset -= 1.f;
                                rotate_points = true;
                            }
                        }
                        else {
                            if (point_scale - 1 != 0) {
                                point_scale--;
                            }
                        }
                    }
                }
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                // object movement
                if (e.key.key == SDLK_A) {
                    std::cout << "A KEY PRESSED" << std::endl;
                    x_pos += movement_step;
                }
                if (e.key.key == SDLK_D) {
                    std::cout << "D KEY PRESSED" << std::endl;
                    x_pos -= movement_step;
                }
                if (e.key.key == SDLK_W) {
                    std::cout << "W KEY PRESSED" << std::endl;
                    y_pos += movement_step;
                }
                if (e.key.key == SDLK_S) {
                    std::cout << "S KEY PRESSED" << std::endl;
                    y_pos -= movement_step;
                }
                // object offset
                if (e.key.key == SDLK_EQUALS) {
                    std::cout << "+ KEY PRESSED" << std::endl;
                    std::cout << "CURRENT SCALE: " << z_offset << std::endl;
                    if (z_offset != 0.0f)
                        z_offset -= 5.f;
                }
                if (e.key.key == SDLK_MINUS) {
                    std::cout << "- KEY PRESSED" << std::endl;
                    std::cout << "CURRENT SCALE: " << z_offset << std::endl;
                    z_offset += 5.f;
                }
                // change color / fov on scroll movement
                if (e.key.key == SDLK_M) {
                    std::cout << "M KEY PRESSED" << std::endl;
                    mouse_wheel_action = 0;
                }
                if (e.key.key == SDLK_N) {
                    std::cout << "N KEY PRESSED" << std::endl;
                    mouse_wheel_action = 1;
                }
                // change color selection
                if (e.key.key == SDLK_V) {
                    current_color++;
                    if (current_color > 3) {
                        current_color = 0;
                    }
                }
                if (e.key.key == SDLK_B) {
                    current_color--;
                    if (current_color < 0) {
                        current_color = 3;
                    }
                }
                if (e.key.key == SDLK_H) {
                    show_help_box = !show_help_box;
                }
                // ONLY FOR 3D
                if (!show_2d) {
                    // object rotation
                    if (e.key.key == SDLK_KP_8 || e.key.key == SDLK_UP) {
                        std::cout << "8 || UP KEY PRESSED" << std::endl;
                        projection.angle_x += rotation_angle;
                    }
                    if (e.key.key == SDLK_KP_2 || e.key.key == SDLK_DOWN) {
                        std::cout << "2 || DOWN KEY PRESSED" << std::endl;
                        projection.angle_x -= rotation_angle;
                    }
                    if (e.key.key == SDLK_KP_4 || e.key.key == SDLK_LEFT) {
                        std::cout << "4 || LEFT KEY PRESSED" << std::endl;
                        projection.angle_y -= rotation_angle;
                    }
                    if (e.key.key == SDLK_KP_6 || e.key.key == SDLK_RIGHT) {
                        std::cout << "4 || LEFT KEY PRESSED" << std::endl;
                        projection.angle_y += rotation_angle;
                    }
                    if (e.key.key == SDLK_KP_7 || e.key.key == SDLK_Q) {
                        std::cout << "7 || Q KEY PRESSED" << std::endl;
                        projection.angle_z -= rotation_angle;
                    }
                    if (e.key.key == SDLK_KP_9 || e.key.key == SDLK_E) {
                        std::cout << "9 || R KEY PRESSED" << std::endl;
                        projection.angle_z += rotation_angle;
                    }
                    // reset to set state
                    if (e.key.key == SDLK_R) {
                        projection.angle_x = 180.0f;
                        projection.angle_y = 0.0f;
                        z_offset = 6.0f;
                        x_pos = 1.0f;
                        y_pos = 1.0f;
                    }
                    projection.update_rotation_matrix_angle();
                    rotate_points = true;
                }
                else {
                    if (e.key.key == SDLK_0) {
                        if (add_new_points) {
                            add_new_points = false;
                            move_points = true;
                        }
                        else {
                            add_new_points = true;
                            move_points = false;
                        }
                    }
                    if (e.key.key == SDLK_R) {
                        x_pos = 0.0f;
                        y_pos = 0.0f;
                        point_scale = 1.0f;
                    }
                    if (e.key.key == SDLK_T) {
                        if (bezier_curve.points_2d.size() > 0) {
                            bezier_curve.points_2d.pop_back();
                            bezier_curve.calc_bezier_curve();
                        }
                    }
                }
            } 
        }
        // ONLY FOR 3D
        if (!show_2d && rotate_points) {
            // 3d to 2d projection
            projection.rot_mat_xy = Matrices::multiply_matrices(projection.rot_mat_y, projection.rot_mat_x);
            projection.rot_mat = Matrices::multiply_matrices(projection.rot_mat_z, projection.rot_mat_xy);
            std::vector<std::vector<Point_3d>> copy_of_control_points_3d = bezier_surface.control_points_3d;
            for (auto it = copy_of_control_points_3d.begin(); it != copy_of_control_points_3d.end(); it++) {
                Matrices::multiply_matrix_vector(*it, bezier_surface.rotated_points, projection.rot_mat);
                bezier_surface.translate_points(bezier_surface.rotated_points, z_offset);
                Matrices::multiply_matrix_vector(bezier_surface.rotated_points, bezier_surface.projected_points, projection.proj_mat);
                bezier_surface.scale_into_view(x_pos, y_pos);
                *it = bezier_surface.projected_points;
            }
            bezier_surface.prepare_3d_points(copy_of_control_points_3d);
            rotate_points = false;
        }
     
        // clear screen
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.b, bg_color.g, bg_color.a);
        SDL_RenderClear(renderer);

        if (!show_2d) {
            // render surfaces
            bezier_surface.render_bezier_surfaces(renderer, x_pos, y_pos, color);
        }
        else {
            // render curves
            SDL_SetRenderScale(renderer, point_scale, point_scale);
            bezier_curve.render_bezier_curve(renderer, x_pos, y_pos, point_scale, color);
            bezier_curve.render_2d_control_points(renderer, x_pos, y_pos, point_scale);
            SDL_RenderFillRect(renderer, &mouse_hitbox);
        }
        if (show_help_box) {
            Help_Box::render_help_box(renderer, font, current_color, mouse_wheel_action, show_2d);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}