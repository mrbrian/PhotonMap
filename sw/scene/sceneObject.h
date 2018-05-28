#pragma once

#include <algebra.h>
#include <material.h>

class SceneObject
{
public:
	virtual ~SceneObject();

    virtual char const *type();
    virtual void Transform(Matrix4x4 m);
    virtual void point_on_surface(Point3D &pos, Vector3D &norm);
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n);

    Material *material;
    Vector3D normal;
};
