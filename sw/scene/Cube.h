#include <algebra.h>
#include <I_SceneObject.h>

class Material;
class Quad;

class Cube : public I_SceneObject
{
public:
    Quad **faces;    // array of Quad pointers

    char const *type();
    ~Cube() ;
    Cube(Point3D center, float scale, Material *mat);
    void Transform(Matrix4x4 m);
    double intersect(Point3D o, Vector3D v, Vector3D *n);
	virtual void point_on_surface(Point3D &pos, Vector3D &norm);
};

