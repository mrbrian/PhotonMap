#include <algebra.h>
#include <I_SceneObject.h>

class Triangle;

class Pyramid : public I_SceneObject
{
public:
    Triangle **faces;    // array of Quad pointers

    char const *type();
    Pyramid(Point3D center, float scale, Material *mat);
    void Transform(Matrix4x4 m);
    double intersect(Point3D o, Vector3D v, Vector3D *n);
};
