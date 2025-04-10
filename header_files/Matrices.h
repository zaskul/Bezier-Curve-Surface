#pragma once
#include "Point_3d.h"
#include <vector>
namespace Matrices {
    struct Matrix4x4 {
        float m[4][4];
        Matrix4x4() : m{} {};
    };
    Matrix4x4 multiply_matrices(const Matrix4x4& a, const Matrix4x4& b);
    void multiply_matrix_vector(std::vector<Point_3d>& i, std::vector<Point_3d>& o, Matrix4x4& m);
}