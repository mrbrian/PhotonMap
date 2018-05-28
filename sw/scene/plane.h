#include <algebra.h>

class Plane
{
public:
    Point3D *points;

    Plane(Point3D a, Point3D b, Point3D c, Point3D d);
    void Transform(Matrix4x4 m);
    Point3D ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight);
};
