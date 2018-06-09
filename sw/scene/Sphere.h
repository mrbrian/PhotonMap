#pragma once

#include "algebra.h"
#include "Material.h"
#include "polyroots.h"
#include "misc.h"
#include <I_SceneObject.h>

class Sphere : public I_SceneObject
{
public:
    Point3D center;
    double radius;

    char const *type();
    void Transform(Matrix4x4 m);
    Sphere(Point3D p, double r, Material *mat);

    double intersect(Point3D o, Vector3D v, Vector3D *n);
};
