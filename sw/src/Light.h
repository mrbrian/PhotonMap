#ifndef LIGHT
#define LIGHT

#include <algebra.h>
#include <vector>

class photon;

class Light
{
public:
    Color clr;
    Point3D position;
    double watts;

    Light(Point3D pos, Color c, double in_watts);
    virtual void emit_photons(int to_emit, float energy, std::vector<photon*> *out_photons);
    virtual double intersect(Point3D o, Vector3D v, Vector3D *n);

    virtual void Transform(Matrix4x4 m)
    {
        position = m * position;
    }
};

#endif //LIGHT
