#pragma once

#include "algebra.h"

class Material
{
public:
    virtual ~Material() { }
    Material() { p = 0; }
    Material(Color a, Color d, Color s, double p, Color r);
    Material(Color a, Color d, Color s, double p);

    // accessor for diffuse color
    virtual Color GetKd(Point3D p)
    {
        p = Point3D();  // get rid of warning
        return Kd;
    }

    Color Ka;      // ambient coefficient
    Color Ks;      // specular coefficient
    double p;       // specular power
    Color Kr;      // reflection coefficient
    Color Kd;      // only access kd through accessor func
};

class ChromeMaterial : public Material
{
public:
    ChromeMaterial()
        : Material( Color(0,0,0), Color(0,0,0), Color(1,1,1), 100, Color(1,1,1))
    {
    }
};

class CheckerMaterial : public Material
{
public:
    float squareSize;      // size of the checkers
    CheckerMaterial(Color a, Color d, Color s, double in_p, Color r, float sc);
    Color GetKd(Point3D p);
};
