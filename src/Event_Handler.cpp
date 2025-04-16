#include "../header_files/Event_Handler.h"
#include <SDL3/SDL.h>
#include "../header_files/Bezier_Surface.h"
#include "../header_files/Bezier_Curve.h"
#include "../header_files/Render_Color.h"
#include "../header_files/Perspective_Projection.h"
#include "../header_files/Point_2d.h"
#include "../header_files/Point_3d.h"
#include "../header_files/Config.h"
#include <iostream>

// 2D curve move/add
bool add_new_points = true;
std::array<Point_2d, 4> new_curve = {};
size_t points_in_array = 0;
bool is_clicked = false;
bool move_points = false;
Point_2d point_selected;
bool change_point_pos = true;
bool point_moved = false;


Event_Handler::Event_Handler(
    Bezier_Surface &bs,
    Bezier_Curve &bc,
    Perspective_Projection &projection,
    Render_Color &color
    ) : 
    color(color),
    bs(bs),
    bc(bc),
    projection(projection)
{
    current_color = 0;
    mouse_wheel_action = 0;
    point_scale = 1.0f;
    x_pos = 1.0f;
    y_pos = 1.0f;
    z_offset = 6.0f;
    rotate_points = true;
    show_2d = true;
    quit = false;
    show_help_box = true;
}

void Event_Handler::handle_events() {
	if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		handle_mouse_button_down();
	} 
	else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
		handle_mouse_button_up();
	} 
	else if (e.type == SDL_EVENT_MOUSE_WHEEL) {
		handle_mouse_wheel();
	} 
	else if (e.type == SDL_EVENT_KEY_DOWN) {
		handle_key_down();
	}
	else if (e.type == SDL_EVENT_QUIT) {
		quit = true;
	}
}

void Event_Handler::handle_key_down() {
    {
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
                if (bc.points_2d.size() > 0) {
                    bc.points_2d.pop_back();
                    bc.calc_bezier_curve();
                }
            }
        }
    }
}

void Event_Handler::handle_mouse_wheel() {
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

void Event_Handler::handle_mouse_button_down() {
    if (show_2d) {
        // move control point
        mouse_hitbox = { e.button.x - mouse_hitbox_size / 2 - x_pos, e.button.y - mouse_hitbox_size / 2 - y_pos, mouse_hitbox_size, mouse_hitbox_size };
        if (move_points) {
            point_selected = bc.check_if_control_point_in_range(mouse_hitbox);
            std::cout << "x :" << point_selected.x << " y: " << point_selected.y << std::endl;
            if (point_selected.x != -1.0f && point_selected.y != -1.0f && !point_moved) {
                change_point_pos = true;
                move_points = false;
            }
        }
        else if (change_point_pos && !add_new_points) {
            std::vector<std::array<Point_2d, 4>>::iterator iter = bc.points_2d.begin() + point_selected.x;
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

void Event_Handler::handle_mouse_button_up() {
    if (show_2d) {
        is_clicked = false;
        // create a straight line
        if (!change_point_pos && !add_new_points) {
            bc.calc_bezier_curve();
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
            bc.points_2d.push_back(new_curve);
            bc.calc_bezier_curve();
            points_in_array = 0;
        }
    }

}
