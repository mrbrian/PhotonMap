#pragma once

#include <algebra.h>
#include <Light.h>
#include <vector>

class I_SceneObject;

class LightObject : public Light
{
public:
    LightObject(Point3D pos, Color c, double in_watts, I_SceneObject *obj);
    virtual ~LightObject();

    void emit_photons(int to_emit, float energy, std::vector<photon*> *out_photons) override;
    double intersect(Point3D o, Vector3D v, Vector3D *n) override;

    I_SceneObject *obj;
};
