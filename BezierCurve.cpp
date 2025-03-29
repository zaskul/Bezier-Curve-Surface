#include <iostream>
#include <SDL3/SDL.h>
#include <vector>
#include <array>
#include <fstream>
#include <string>
#include <regex>
#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL3_ttf/SDL_ttf.h>

struct Point_2D {
    float x;
    float y;
    Point_2D(float x, float y) : x(x), y(y) {};
    Point_2D() : x(0.0f), y(0.0f) {};
};

struct Point_3D {
    Point_2D p_2d;
    float z;

    Point_3D(float x, float y, float z) : p_2d{x, y}, z(z) {};

    Point_3D() : p_2d{ 0, 0 }, z(0) {};
};

struct Render_Color {
    int r;
    int g;
    int b;
    int a;
    Render_Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {};
};

struct Matrix4x4 {
    float m[4][4] = { 0 };
};

struct Rotation_State {
    float angleX;
    float angleY;
    float angleZ;
};

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
std::vector<Point_2D> calc_bezier_curve(std::vector<std::array<Point_2D, 4>> xy, float px_density) {
    std::vector<Point_2D> res_vector;
    Point_2D point = Point_2D();
    for (auto vec_it = xy.begin(); vec_it != xy.end(); vec_it++) {
        // read individual curve
        float x[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float y[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        int i = 0;
        std::array<Point_2D, 4>::iterator check_dist = vec_it->begin();
        if (std::distance(check_dist, vec_it->end()) < 4) return res_vector;
        for (auto it = vec_it->begin(); it != vec_it->end(); it++) {
            x[i] = it->x;
            y[i] = it->y;
            i++;
        }
        // calculate the curve
        for (float t = 0; t <= 1.0f; t += px_density) {
            point.x = calc_bernstein(t, 0) * x[0] +
                calc_bernstein(t, 1)* x[1] +
                    calc_bernstein(t, 2) * x[2] +
                    calc_bernstein(t, 3) * x[3];
                point.y = calc_bernstein(t, 0) * y[0] +
                    calc_bernstein(t, 1) * y[1] +
                    calc_bernstein(t, 2) * y[2] +
                    calc_bernstein(t, 3) * y[3];
                std::cout << "x: " << point.x << "y: " << point.y << std::endl;
                res_vector.push_back(point);
        }
    }
    return res_vector;
}

void render_bezier_curve(SDL_Renderer* renderer, std::vector<Point_2D> pts, float x_pos, float y_pos, float scale) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for (auto it = pts.begin(); it != pts.end(); it++) {
        it->x += x_pos / 1.5f / scale;
        it->y += y_pos / 1.5f / scale;
        SDL_SetRenderScale(renderer, scale, scale);
        SDL_RenderPoint(renderer, it->x, it->y);
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    }
}

std::vector<Point_3D> calc_bezier_surfaces(std::vector<Point_3D> xyz, float px_density) {
    std::vector<Point_3D> processed_points;
    for (float v = 0; v <= 1; v += px_density) {
        for (float w = 0; w <= 1; w += px_density) {
            Point_3D point = { 0, 0, 0 };
            std::vector<Point_3D>::iterator it = xyz.begin();
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    point.p_2d.x += it->p_2d.x * calc_bernstein(v, i) * calc_bernstein(w, j);
                    point.p_2d.y += it->p_2d.y * calc_bernstein(v, i) * calc_bernstein(w, j);
                    point.z += it->z * calc_bernstein(v, i) * calc_bernstein(w, j);
                    it++;
                }
            }
            processed_points.push_back(point);
        }
    }
    return processed_points;
}

void render_bezier_surfaces(SDL_Renderer* renderer, std::vector<Point_3D>& pts, Render_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (auto it = pts.begin(); it != pts.end(); it++) {
        Point_3D point = *it;
        SDL_RenderPoint(renderer, point.p_2d.x, point.p_2d.y);
    }
}

Matrix4x4 multiply_matrices(const Matrix4x4& a, const Matrix4x4& b) {
    Matrix4x4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            result.m[i][j] += a.m[i][0] * b.m[0][j];
            result.m[i][j] += a.m[i][1] * b.m[1][j];
            result.m[i][j] += a.m[i][2] * b.m[2][j];
            result.m[i][j] += a.m[i][3] * b.m[3][j];
        }
    }
    return result;
}

void multiply_matrix_vector(std::vector<Point_3D>& i, std::vector<Point_3D>& o, Matrix4x4& m) {
    o.resize(i.size());
    std::vector<Point_3D>::iterator output_iter = o.begin();
    for (auto it = i.begin(); it != i.end(); it++, output_iter++) {
        float x = it->p_2d.x * m.m[0][0] + it->p_2d.y * m.m[1][0] + it->z * m.m[2][0] + m.m[3][0];
        float y = it->p_2d.x * m.m[0][1] + it->p_2d.y * m.m[1][1] + it->z * m.m[2][1] + m.m[3][1];
        float z = it->p_2d.x * m.m[0][2] + it->p_2d.y * m.m[1][2] + it->z * m.m[2][2] + m.m[3][2];
        float w = it->p_2d.x * m.m[0][3] + it->p_2d.y * m.m[1][3] + it->z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f) {
            x /= w;
            y /= w;
            z /= w;
        }

        output_iter->p_2d.x = x;
        output_iter->p_2d.y = y;
        output_iter->z = z;
    }
}

std::vector<Point_3D> translate_points(std::vector<Point_3D> pts, float z_offset) {
    std::vector<Point_3D> output;

    for (auto it = pts.begin(); it != pts.end(); it++) {
        float z = it->z + z_offset;
        output.push_back(Point_3D(it->p_2d.x, it->p_2d.y, z));
    }
    return output;
}

std::vector <Point_3D> scale_into_view(std::vector <Point_3D> pts, int win_height, int win_width, float x_pos, float y_pos) {
    std::vector<Point_3D> output;

    Point_3D point;
    for (auto it = pts.begin(); it != pts.end(); it++) {
        point.p_2d.x = (it->p_2d.x + x_pos) * 0.5f * (float)win_width;
        point.p_2d.y = (it->p_2d.y + y_pos ) * 0.5f * (float)win_height;
        point.z = it->z;
        output.push_back(point);
    }
    return output;
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

std::vector<std::vector<Point_3D>> read_file(std::string file_name) {
    std::string file_path = "Objects/" + file_name;
    std::ifstream inputFile(file_path);
    if (!inputFile.is_open()) {
        std::cerr << "File could not be opened" << std::endl;
    }

    std::string line;
    std::getline(inputFile, line);
    int num_of_patches = stoi(line);
    
    std::vector<std::vector<Point_3D>> points_3d(num_of_patches);

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
            
            points_3d[i][j] = Point_3D(temp_x, temp_y, temp_z);
        }
    }
    inputFile.close();
    return points_3d;
}

void update_rotation_matrix_angle(Matrix4x4* mz, Matrix4x4* my, Matrix4x4* mx, Rotation_State* state) {
    // rotate Z
    if (state->angleZ < 0.0f) 
        state->angleZ = 360.0f + state->angleZ;
    if (state->angleZ > 360.0f)
        state->angleZ = state->angleZ - 360.0f;

    float thetaZ = state->angleZ * M_PI / 180.0f;
    mz->m[0][0] = cosf(thetaZ);
    mz->m[0][1] = sinf(thetaZ);
    mz->m[1][0] = -sinf(thetaZ);
    mz->m[2][0] = cosf(thetaZ);
    mz->m[2][2] = 1;
    mz->m[3][3] = 1;

    //rotate Y
    if (state->angleY < 0.0f)
        state->angleY = 360.0f + state->angleY;
    if (state->angleY > 360.0f)
        state->angleY = state->angleY - 360.0f;

    float thetaY = state->angleY * M_PI / 180.0f;
    my->m[0][0] = cosf(thetaY);
    my->m[0][2] = -sinf(thetaY);
    my->m[1][1] = 1;
    my->m[2][0] = sinf(thetaY);
    my->m[2][2] = cosf(thetaY);
    my->m[3][3] = 1;
    
    // rotate X
    if (state->angleX < 0.0f)
        state->angleX = 360.0f + state->angleX;
    if (state->angleX > 360.0f)
        state->angleX = state->angleX - 360.0f;

    float thetaX = state->angleX * M_PI / 180.0f;
    mx->m[0][0] = 1;
    mx->m[1][1] = cosf(thetaX);
    mx->m[1][2] = sinf(thetaX);
    mx->m[2][1] = -sinf(thetaX);
    mx->m[2][2] = cosf(thetaX);
    mx->m[3][3] = 1;

    std::cout << "x rad: " << thetaX << "; x angle:" << state->angleX << std::endl;
    std::cout << "y rad: " << thetaY << "; y angle:" << state->angleY << std::endl;
    std::cout << "z rad: " << thetaZ << "; z angle:" << state->angleZ << std::endl;
}
void update_projection_matrix(Matrix4x4& pm, float ar, float fov) {
    float fov_rad = 1.0f / tanf(fov * 0.5f / 180.f * M_PI);
    pm.m[0][0] = ar * fov_rad;
    pm.m[1][1] = fov_rad;
}

void init_3d_points(std::vector<Point_3D>& pts, std::string file_name, float px_density) {
    std::vector<std::vector<Point_3D>> points = read_file(file_name);
    // create a combined 3d point vector from the file
    for (auto it = points.begin(); it != points.end(); it++) {
        std::vector<Point_3D> temp_vec = calc_bezier_surfaces(*it, px_density);
        pts.reserve(pts.size() + temp_vec.size());
        pts.insert(pts.end(), temp_vec.begin(), temp_vec.end());
    }
}

void init_2d_points(std::vector<Point_2D>& pts, float px_density) {
    std::vector<std::array<Point_2D, 4>> points2d;
    points2d.push_back({ {{ 174.f, 59.f }, { 146.f, 10.f }, { 131.f, 76.f }, { 131.f, 86.f } } });
    points2d.push_back({ { { 131.f, 86.f }, { 131.f, 96.f }, { 124.f, 195.f }, { 119.f, 209.f } } });
    points2d.push_back({ { { 119.f, 209.f }, { 110.f, 227.f }, { 84.f, 228.f }, { 83.f, 217.f } } });
    points2d.push_back({ { { 83.f, 217.f }, { 79.f, 180.f}, { 136.f, 215.f }, { 142.f, 223.f } } });
    points2d.push_back({ { { 142.f, 223.f }, { 154.f, 239.f }, { 195.f, 204.f}, { 193.f, 197.f } } });
    float x_offset = 30.f;
    float y_offset = 60.f;
    points2d.push_back({ {{(88.f + x_offset) / 2.f, (193.f + y_offset) / 2.f}, {(294.f + x_offset) / 2.f, (113.f + y_offset) / 2.f}, {(189.f - x_offset) / 2.f, (282.f + y_offset) / 2.f}, {(394.f - x_offset) / 2.f, (188.f + y_offset) / 2.f}} });
    pts = calc_bezier_curve(points2d, px_density);
}

// render keybindings help box
void render_text(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int text_width = surface->w;
    int text_height = surface->h;
    SDL_DestroySurface(surface);

    SDL_FRect dest_rect = { (float)x, (float)y, (float)text_width, (float)text_height };
    SDL_RenderTexture(renderer, texture, NULL, &dest_rect);

    SDL_DestroyTexture(texture);
}
void render_help_box(SDL_Renderer* renderer, TTF_Font* font, short current_color, short mouse_wheel_action) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color variable_color = { 0, 255, 0 , 255 };
    std::string current_color_array[4] = { "Red", "Green", "Blue", "Alpha" };
    std::string mouse_wheel_action_array[2] = {"Color", "FOV"};
    int x = 20, y = 20;  // Position of the help box
    render_text(renderer, "CONTROLS:", x, y, white, font);
    y += 30;
    render_text(renderer, "W / S  - Move Up/Down", x, y, white, font);
    y += 25;
    render_text(renderer, "A / D  - Move Left/Right", x, y, white, font);
    y += 25;
    render_text(renderer, "+ / -  - Offset object in the Z axis", x, y, white, font);
    y += 25;
    render_text(renderer, "M / N  - Change color / FOV on scroll", x, y, white, font);
    y += 25;
    render_text(renderer, "V / B Select color to change R>G>B", x, y, white, font);
    y += 25;
    render_text(renderer, "Up / Down arrow - Rotate X Axis", x, y, white, font);
    y += 25;
    render_text(renderer, "Left / Right arrow - Rotate Y Axis", x, y, white, font);
    y += 25;
    render_text(renderer, "Q / E  - Rotate Z Axis", x, y, white, font);
    y += 25;
    render_text(renderer, "R - Reset Transformations", x, y, white, font);
    y += 25;
    std::string col = "Current color: " + current_color_array[current_color];
    render_text(renderer, col, x, y, variable_color, font);
    y += 25;
    std::string mwa = "Current action: Change " + mouse_wheel_action_array[mouse_wheel_action];
    render_text(renderer, mwa, x, y, variable_color, font);
    y += 25;
    render_text(renderer, "H - Show/Hide this box", x, y, white, font);
}

void change_color(Render_Color& color , short current_color, int color_step) {
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

int main()
{
    // init window and renderer
    int width = 1000;
    int height = 1000;

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

    // init font
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize!" << std::endl;
    }
    TTF_Font* font = TTF_OpenFont("ThirdParty/ARIAL.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font" << std::endl;
    }

    // init rotation matrices
    Matrix4x4 rot_mat_x, rot_mat_z, rot_mat_y, rot_mat_xy, rot_mat, proj_mat;

    // init rotation state
    Rotation_State state;

    // set initial position
    state.angleX = 90.0f;
    state.angleY = 90.0f;
    state.angleZ = 90.0f;
    update_rotation_matrix_angle(&rot_mat_z, &rot_mat_y, &rot_mat_x, &state);

    // init projection matrix
    float aspect_ratio = (float)height / (float)width;
    float near = 0.1f;
    float far = 1000.0f;
    float fov = 90.0f;
    float fov_rad = 1.0f / tanf(fov * 0.5f / 180.f * M_PI);

    proj_mat.m[0][0] = aspect_ratio * fov_rad;
    proj_mat.m[1][1] = fov_rad;
    proj_mat.m[2][2] = far / (far - near);
    proj_mat.m[3][2] = (-far * near) / (far - near);
    proj_mat.m[2][3] = 1.0f;
    proj_mat.m[3][3] = 0.0f;

    // init variables
    bool quit = false;
    SDL_Event e;
    float rotation_angle = 10.0f;
    float px_density = 0.05f;
    float x_pos = 1.0f;
    float y_pos = 1.0f;
    float z_offset = 6.0f;
    float movement_step = 0.01f;
    int color_step = 15;
    short current_color = 0;
    short mouse_wheel_action = 0;
    Render_Color color = { 255, 0, 0, 255 };
    Render_Color bg_color = { 0, 0, 0, 255 };
    bool show_help_box = true;
    
    // init point vectors
    std::vector<Point_3D> processed_points_3d, rotated_points, projected_points;
    init_3d_points(processed_points_3d, "teapotCGA.txt", px_density);
    std::vector<Point_2D> processed_points_2d;
    //init_2d_points(processed_points_2d, px_density);
    
    // amount of points
    std::cout << processed_points_3d.size() << std::endl;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (e.type == SDL_EVENT_MOUSE_WHEEL) {
                if (e.wheel.y > 0) {
                    std::cout << "UP" << std::endl;
                    if (mouse_wheel_action == 0) {
                        change_color(color, current_color, color_step);
                    }
                    else if (mouse_wheel_action == 1) {
                        fov++;
                        update_projection_matrix(proj_mat, aspect_ratio, fov);
                    }
                }
                if (e.wheel.y < 0) {
                    std::cout << "DOWN" << std::endl;
                    if (mouse_wheel_action == 0) {
                        change_color(color, current_color, -color_step);
                    }
                    else if (mouse_wheel_action == 1) {
                        fov--;
                        update_projection_matrix(proj_mat, aspect_ratio, fov);
                    }
                }
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                // object movement
                if (e.key.key == SDLK_A) {
                    std::cout << "A KEY PRESSED" << std::endl;
                    x_pos -= movement_step;
                }
                if (e.key.key == SDLK_D) {
                    std::cout << "D KEY PRESSED" << std::endl;
                    x_pos += movement_step;
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
                        z_offset -= 1.f;
                }
                if (e.key.key == SDLK_MINUS) {
                    std::cout << "- KEY PRESSED" << std::endl;
                    std::cout << "CURRENT SCALE: " << z_offset << std::endl;
                    z_offset += 1.f;
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
                // object rotation
                if (e.key.key == SDLK_KP_8 || e.key.key == SDLK_UP) {
                    std::cout << "8 || UP KEY PRESSED" << std::endl;
                    state.angleX += rotation_angle;
                }
                if (e.key.key == SDLK_KP_2 || e.key.key == SDLK_DOWN) {
                    std::cout << "2 || DOWN KEY PRESSED" << std::endl;
                    state.angleX -= rotation_angle;
                }
                if (e.key.key == SDLK_KP_4 || e.key.key == SDLK_LEFT) {
                    std::cout << "4 || LEFT KEY PRESSED" << std::endl;
                    state.angleY -= rotation_angle;
                }
                if (e.key.key == SDLK_KP_6 || e.key.key == SDLK_RIGHT) {
                    std::cout << "4 || LEFT KEY PRESSED" << std::endl;
                    state.angleY += rotation_angle;
                }
                if (e.key.key == SDLK_KP_7 || e.key.key == SDLK_Q) {
                    std::cout << "7 || Q KEY PRESSED" << std::endl;
                    state.angleZ -= rotation_angle;
                }
                if (e.key.key == SDLK_KP_9 || e.key.key == SDLK_E) {
                    std::cout << "9 || R KEY PRESSED" << std::endl;
                    state.angleZ += rotation_angle;
                }
                if (e.key.key == SDLK_H) {
                    show_help_box = !show_help_box;
                }
                // reset to set state
                if (e.key.key == SDLK_R) {
                    state.angleZ = 90.0f;
                    state.angleX = 180.0f;
                    state.angleY = 0.0f;
                    z_offset = 6.0f;
                }
                update_rotation_matrix_angle(&rot_mat_z, &rot_mat_y, &rot_mat_x, &state);
            }
        }

        // 3d to 2d projection
        rot_mat_xy = multiply_matrices(rot_mat_y, rot_mat_x);
        rot_mat = multiply_matrices(rot_mat_z, rot_mat_xy);
        multiply_matrix_vector(processed_points_3d, rotated_points, rot_mat);
        rotated_points = translate_points(rotated_points, z_offset);
        multiply_matrix_vector(rotated_points, projected_points, proj_mat);
        projected_points = scale_into_view(projected_points, height, width, x_pos, y_pos);
        std::sort(projected_points.begin(), projected_points.end(), [](Point_3D& p1, Point_3D& p2) {
            return p1.z > p2.z;
        });
            
        // clear screen
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.b, bg_color.g, bg_color.a);
        SDL_RenderClear(renderer);

        // render surfaces
        render_bezier_surfaces(renderer, projected_points, color);

        // render curve
        //render_bezier_curve(renderer, processed_points_2d, x_pos, y_pos, 3.0f);
        if (show_help_box) {
            render_help_box(renderer, font, current_color, mouse_wheel_action);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}