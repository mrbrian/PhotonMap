#ifndef CAMERA_H
#define CAMERA_H

#include "algebra.h"

class Plane
{
public:
    Point3D *points;

    Plane(Point3D a, Point3D b, Point3D c, Point3D d)
    {
        points = new Point3D[4] {a,b,c,d};
    }

    void Transform(Matrix4x4 m)
    {
        for (int i = 0; i < 4 ;i ++)
        {
            points[i] = m * points[i];
        }
    }

    Point3D ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight)
    {
        Point3D topleft  = points[0];
        Point3D topright = points[3];
        Point3D botleft  = points[1];

        Vector3D right = topright - topleft;
        Vector3D down  = botleft - topleft;

        double h_pct = (float)x / imgWidth;
        double v_pct = (float)y / imgHeight;

        Point3D result = (topleft + (h_pct * right) + (v_pct * down));
        return result;
    }
};

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
    Plane *calc_img_plane();
};

#endif // CAMERA_H
