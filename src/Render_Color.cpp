#include "../header_files/Render_Color.h"

void Render_Color::change_color(Render_Color& color, short current_color, int color_step) {
    if (current_color == 0) {
        color.r += color_step;
        if (color.r > 255) color.r = 255;
        if (color.r < 0) color.r = 0;
    }
    else if (current_color == 1) {
        color.g += color_step;
        if (color.g > 255) color.g = 255;
        if (color.g < 0) color.g = 0;
    }
    else if (current_color == 2) {
        color.b += color_step;
        if (color.b > 255) color.b = 255;
        if (color.b < 0) color.b = 0;
    }
    else if (current_color == 3) {
        color.a += color_step;
        if (color.a > 255) color.a = 255;
        if (color.a < 0) color.a = 0;
    }
}