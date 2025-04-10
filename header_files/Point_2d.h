#pragma once
struct Point_2d {
public:
	float x;
	float y;

	Point_2d() : x(0.0f), y(0.0f) {};
	Point_2d(float x, float y) : x{ x }, y{ y } {};
};
