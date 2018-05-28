#pragma once

#include "algebra.h"
#include "material.h"
#include "polyroots.h"
#include "misc.h"

class SceneObject
{
public:
    Material *material;
    Vector3D normal;

    virtual char const *type();
    virtual void Transform(Matrix4x4 m);
    virtual void point_on_surface(Point3D &pos, Vector3D &norm);
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n);
};

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

class Quad : public SceneObject
{
public:
    Triangle **tris;    // array of triangle pointers
    Point3D *points;

    char const *type() override;
    ~Quad();
    Quad(Point3D a, Point3D b, Point3D c, Point3D d, Material *mat);
    void Transform(Matrix4x4 m) override;
    double intersect(Point3D o, Vector3D v, Vector3D *n) override;
    Point3D ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight);
    void point_on_surface(Point3D &pos, Vector3D &normal) override;
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
