#include <SDL3/SDL.h>
#include "Bezier_Surface.h"
#include "Bezier_Curve.h"
#include "Perspective_Projection.h"
#include "Config.h"
#include "Render_Color.h"
class Event_Handler {
public:
	Event_Handler(Bezier_Surface &bs, Bezier_Curve &bc, Perspective_Projection &projection, Render_Color &color);
	SDL_Event e;
	bool quit, show_2d, rotate_points, show_help_box;
	SDL_FRect mouse_hitbox = { 0.0f, 0.0f, 0.0f, 0.0f };
	Bezier_Surface &bs;
	Bezier_Curve &bc;
	Perspective_Projection &projection;
	Render_Color &color;
	short current_color;
	short mouse_wheel_action;
	float point_scale;
	float x_pos;
	float y_pos;
	float z_offset;
	float mouse_hitbox_size = Config::MOUSE_HITBOX_SIZE;
	float movement_step = Config::MOVEMENT_STEP;
	int color_step = Config::COLOR_STEP;
	float rotation_angle = Config::ROTATION_ANGLE;
	void handle_events();
	void handle_key_down();
	void handle_mouse_wheel();
	void handle_mouse_button_down();
	void handle_mouse_button_up();
};