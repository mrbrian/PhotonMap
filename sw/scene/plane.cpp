#include <plane.h>

Plane::Plane(Point3D a, Point3D b, Point3D c, Point3D d)
{
    points = new Point3D[4] {a,b,c,d};
}

void Plane::Transform(Matrix4x4 m)
{
    for (int i = 0; i < 4 ;i ++)
    {
        points[i] = m * points[i];
    }
}

Point3D Plane::ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight)
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
