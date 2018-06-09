#include <LightObject.h>
#include <scene.h>
#include <photon.h>

LightObject::LightObject(Point3D pos, Color c, double in_watts, I_SceneObject *o)
    : Light(pos, c, in_watts)
{
    obj = o;

}

LightObject::~LightObject()
{
    delete obj;
}

double LightObject::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    return obj->intersect(o, v, n);
}

void LightObject::emit_photons(int to_emit, float energy, std::vector<photon*> *out_photons)
{
    int num_emit = 0;
    while (num_emit < to_emit)
    {
        Vector3D norm;
        Point3D p_pos;
        obj->point_on_surface(p_pos, norm);
        norm.normalize();

        double x;
        double y;
        double z;
        Vector3D dir;
        do
        {
            x = misc::RAND_1();
            y = misc::RAND_1();
            z = misc::RAND_1();
            dir = Vector3D(x, y, z);
        }
        while (dir.length2() > 1);
        dir.normalize();

        photon *p = new photon(p_pos, dir, clr, energy);

        out_photons->push_back(p);
        num_emit++;
    }
}
