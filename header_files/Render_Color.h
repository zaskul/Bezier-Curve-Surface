#pragma once
struct Render_Color {
public:
    int r;
    int g;
    int b;
    int a;

    Render_Color(int r, int g, int b, int a) : r(r), g(g), b(b), a(a) {};
    void change_color(Render_Color& color, short current_color, int color_step);

};