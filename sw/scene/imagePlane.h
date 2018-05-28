#include <algebra.h>

class ImagePlane
{
public:
    Point3D *points;

    ImagePlane(Point3D a, Point3D b, Point3D c, Point3D d);
    ~ImagePlane();
    void Transform(Matrix4x4 m);
    Point3D ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight);
};
