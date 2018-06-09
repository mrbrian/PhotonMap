#pragma once

#include <algebra.h>
#include <I_SceneObject.h>
#include <shapes.h>

class Triangle;

class Quad : public I_SceneObject
{
public:
    Triangle **tris;    // array of triangle pointers
    Point3D *points;

    virtual char const *type() override;
    ~Quad();
    Quad(Point3D a, Point3D b, Point3D c, Point3D d, Material *mat);
    virtual void Transform(Matrix4x4 m) override;
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n) override;
    Point3D ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight);
    virtual void point_on_surface(Point3D &pos, Vector3D &normal) override;
};
