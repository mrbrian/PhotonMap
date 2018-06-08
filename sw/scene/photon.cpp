#include "photon.h"

void photon::set_position(Point3D p)
{
    x = p[0];
    y = p[1];
    z = p[2];
}

void photon::set_direction(Vector3D dir)
{
    if (dir.length2() > 1.1)
        throw ("Normalize plz");
    if (dir.length2() < 0.9)
        throw ("Normalize plz");
    double t = atan2(dir[2], dir[0]);
    t += M_PI;
    t /= (2 * M_PI);
    double t2 = acos(dir[1]);
    t2 /= M_PI;

    phi = (char)((255 * (atan2(dir[2], dir[0]) + M_PI)) / (2 * M_PI));
    theta = (char)(255 * acos(dir[1]) / M_PI);
}

void photon::set_color(Color clr)
{
    color = clr;
}

Color photon::get_color() const
{
    return Color(color);
}

Point3D photon::get_position() const
{
    return Point3D(x, y, z);
}

Vector3D photon::get_direction() const
{
    double ph = ((float)phi * 2 * M_PI) / 255.0 - M_PI;
    double th = (float)theta / 255.0 * M_PI;

    float x = cos(ph) * sin(th);
    float z = sin(ph) * sin(th);
    float y = cos(th);

    return Vector3D(x, y, z);
}
