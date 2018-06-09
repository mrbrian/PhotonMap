#include <algebra.h>
#include <I_SceneObject.h>

class Triangle : public I_SceneObject
{
public:
    Triangle(Point3D p1, Point3D p2, Point3D p3, Material *mat);

    virtual char const *type();
    virtual void Transform(Matrix4x4 m);
    virtual void point_on_surface(Point3D &pos, Vector3D &norm);
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n);
    virtual double area(Point3D a, Point3D b, Point3D c);

    Point3D verts[3];
	Vector3D &normal() const;

private:
	Vector3D normal_;
};
