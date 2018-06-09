#pragma once

#include <algebra.h>

class Material;

class I_SceneObject
{
public:
	virtual ~I_SceneObject();

    virtual char const *type() = 0;
    virtual void Transform(Matrix4x4 m) = 0;
    virtual void point_on_surface(Point3D &pos, Vector3D &norm) = 0;
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n) = 0;

    Material *material;
};
