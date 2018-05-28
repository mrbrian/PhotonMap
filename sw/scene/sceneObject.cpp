#include <sceneObject.h>

char const *SceneObject::type()
{
    return "SceneObject";
}

void SceneObject::Transform(Matrix4x4 m)
{
    m = 0;
}

void SceneObject::point_on_surface(Point3D &pos, Vector3D &norm)
{
    pos = Point3D(-1,-1,-1);
    norm = Vector3D(-1,-1,-1);
}

double SceneObject::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    o = Point3D();  // get rid of warnings
    v = *n;
    return -1;
}
