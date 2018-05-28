#pragma once

#include <algebra.h>

class ImagePlane;

class Camera
{
public:
    int imgWidth;       // image resolution width
    int imgHeight;      // image resolution height
    Point3D position;   // camera position
    Point3D lookAt;     // camera lookat

    float fov;
    double aspect;
    double near;
    double far;
    Matrix4x4 m_view;

    Matrix4x4 *calc_proj_matrix();
    Matrix4x4 get_view_matrix();
    ImagePlane *calc_img_plane();
};
