#include "../header_files/Matrices.h"
#include "../header_files/Config.h"
#include "../header_files/Perspective_Projection.h"
#define _USE_MATH_DEFINES
#include <math.h>

Perspective_Projection::Perspective_Projection() {
    // init rotation state
    angle_x = 180.0f;
    angle_y = 0.0f;
    angle_z = 90.0f;

    // init rotation matrices
    update_rotation_matrix_angle();

    // init projection matrix
    float aspect_ratio = (float)Config::WINDOW_HEIGHT / (float)Config::WINDOW_WIDTH;
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

}

void Perspective_Projection::update_rotation_matrix_angle() {
    // rotate Z
    if (angle_z < 0.0f)
        angle_z = 360.0f + angle_z;
    if (angle_z > 360.0f)
        angle_z = angle_z - 360.0f;

    float thetaZ = angle_z * M_PI / 180.0f;
    rot_mat_z.m[0][0] = cosf(thetaZ);
    rot_mat_z.m[0][1] = sinf(thetaZ);
    rot_mat_z.m[1][0] = -sinf(thetaZ);
    rot_mat_z.m[2][0] = cosf(thetaZ);
    rot_mat_z.m[2][2] = 1;
    rot_mat_z.m[3][3] = 1;

    //rotate Y
    if (angle_y < 0.0f)
        angle_y = 360.0f + angle_y;
    if (angle_y > 360.0f)
        angle_y = angle_y - 360.0f;

    float thetaY = angle_y * M_PI / 180.0f;
    rot_mat_y.m[0][0] = cosf(thetaY);
    rot_mat_y.m[0][2] = -sinf(thetaY);
    rot_mat_y.m[1][1] = 1;
    rot_mat_y.m[2][0] = sinf(thetaY);
    rot_mat_y.m[2][2] = cosf(thetaY);
    rot_mat_y.m[3][3] = 1;

    // rotate X
    if (angle_x < 0.0f)
        angle_x = 360.0f + angle_x;
    if (angle_x > 360.0f)
        angle_x = angle_x - 360.0f;

    float thetaX = angle_x * M_PI / 180.0f;
    rot_mat_x.m[0][0] = 1;
    rot_mat_x.m[1][1] = cosf(thetaX);
    rot_mat_x.m[1][2] = sinf(thetaX);
    rot_mat_x.m[2][1] = -sinf(thetaX);
    rot_mat_x.m[2][2] = cosf(thetaX);
    rot_mat_x.m[3][3] = 1;


}

void Perspective_Projection::update_projection_matrix(Matrices::Matrix4x4& pm, float ar, float fov) {
    float fov_rad = 1.0f / tanf(fov * 0.5f / 180.f * M_PI);
    pm.m[0][0] = ar * fov_rad;
    pm.m[1][1] = fov_rad;
}