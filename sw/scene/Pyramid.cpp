#include <Triangle.h>

char const *Pyramid::type()
{
    return "Cube";
}

Pyramid::Pyramid(Point3D center, float scale, Material *mat)
{
    faces = new Triangle*[6];
    material = mat;
    for (int i = 0; i < 4; i++)
    {
        Triangle *t = new Triangle(Point3D(-1,-1,-1), Point3D(1,-1,-1), Point3D(0,1,0), material);
        faces[i] = t;
        t->Transform(Matrix4x4::rotation(M_PI_2 * i, 'y'));
    }

    faces[4] = new Triangle(Point3D(-1,-1,1), Point3D(-1,-1,-1), Point3D(1,-1,-1), material);
    faces[5] = new Triangle(Point3D(-1,-1,1), Point3D(1,-1,-1), Point3D(1,-1,1), material);
    Transform(
                Matrix4x4::translation(Vector3D(center[0], center[1], center[2])) *
                Matrix4x4::scaling(Vector3D(scale,scale,scale)));
}

void Pyramid::Transform(Matrix4x4 m)
{
    for (int j = 0; j < 6; j++)
    {
        faces[j]->Transform(m);
    }
}

double Pyramid::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    double t_min = INFINITY;
    Vector3D norm;
    Vector3D &n_min = *n;
    for (int i = 0 ; i < 6; i++)
    {
        double t = faces[i]->intersect(o,v,&norm);
        if (t <= t_min && t >= 0)
        {
            t_min = t;
            n_min = norm;
        }
    }
    if (t_min == INFINITY)
        return -1;
    return t_min;
}
