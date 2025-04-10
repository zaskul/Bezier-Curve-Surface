#include "../header_files/Matrices.h"
namespace Matrices {
    Matrix4x4 Matrices::multiply_matrices(const Matrix4x4& a, const Matrix4x4& b) {
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

    void Matrices::multiply_matrix_vector(std::vector<Point_3d>& i, std::vector<Point_3d>& o, Matrix4x4& m) {
        o.resize(i.size());
        std::vector<Point_3d>::iterator output_iter = o.begin();
        for (auto it = i.begin(); it != i.end(); it++, output_iter++) {
            float x = it->x * m.m[0][0] + it->y * m.m[1][0] + it->z * m.m[2][0] + m.m[3][0];
            float y = it->x * m.m[0][1] + it->y * m.m[1][1] + it->z * m.m[2][1] + m.m[3][1];
            float z = it->x * m.m[0][2] + it->y * m.m[1][2] + it->z * m.m[2][2] + m.m[3][2];
            float w = it->x * m.m[0][3] + it->y * m.m[1][3] + it->z * m.m[2][3] + m.m[3][3];

            if (w != 0.0f) {
                x /= w;
                y /= w;
                z /= w;
            }

            output_iter->x = x;
            output_iter->y = y;
            output_iter->z = z;
        }
    }
}