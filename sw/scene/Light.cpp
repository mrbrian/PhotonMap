#include "Light.h"
#include <misc.h>
#include <photon.h>

Light::Light(Point3D pos, Color c, double in_watts)
{
    position = pos;
    clr = c;
    watts = in_watts;
}

void Light::emit_photons(int to_emit, float energy, std::vector<photon*> *out_photons)
{
    int num_emit = 0;
    while (num_emit < to_emit)
    {
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
        photon *p = new photon(position, dir, clr, energy);       // only using the diffuse color..(?)

        out_photons->push_back(p);
        num_emit++;
    }
}


double Light::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    o = Point3D();  // get rid of warnings
    v = *n;
    return -1;
}
