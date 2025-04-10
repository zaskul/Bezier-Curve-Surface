#pragma once
struct Point_3d {
public:
    float x;
    float y;
    float z;

    Point_3d(float x, float y, float z) : x{ x }, y{ y }, z(z) {};
    Point_3d() : x(0), y(0), z(0) {};
};