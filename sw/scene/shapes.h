#pragma once

#include "algebra.h"
#include "material.h"
#include "polyroots.h"
#include "misc.h"
#include <sceneObject.h>

class Quad;

class Triangle : public SceneObject
{
public:
    Point3D verts[3];

    char const *type();
    Triangle(Point3D p1, Point3D p2, Point3D p3, Material *mat);
    double area(Point3D a, Point3D b, Point3D c);
    double intersect(Point3D o, Vector3D v, Vector3D *n);
    void Transform(Matrix4x4 m);
};

class Sphere : public SceneObject
{
public:
    Point3D center;
    double radius;

    char const *type();
    void Transform(Matrix4x4 m);
    Sphere(Point3D p, double r, Material *mat);

    double intersect(Point3D o, Vector3D v, Vector3D *n);
};


class Cube : public SceneObject
{
public:
    Quad **faces;    // array of Quad pointers

    char const *type();
    ~Cube() ;
    Cube(Point3D center, float scale, Material *mat);
    void Transform(Matrix4x4 m);
    double intersect(Point3D o, Vector3D v, Vector3D *n);
};



class Pyramid : public SceneObject
{
public:
    Triangle **faces;    // array of Quad pointers

    char const *type();
    Pyramid(Point3D center, float scale, Material *mat);
    void Transform(Matrix4x4 m);
    double intersect(Point3D o, Vector3D v, Vector3D *n);
};
