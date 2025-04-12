#pragma once
#include "Matrices.h"
#include "Bezier_Surface.h"
class Perspective_Projection {
public:
    float angle_x, angle_y, angle_z;
    Matrices::Matrix4x4 rot_mat_x, rot_mat_z, rot_mat_y, rot_mat_xy, rot_mat, proj_mat;
    Perspective_Projection();
    void update_rotation_matrix_angle();
    void update_projection_matrix(Matrices::Matrix4x4& pm, float ar, float fov);
    void apply_projection(Bezier_Surface& bs, float x_pos, float y_pos, float z_offset);
};
    