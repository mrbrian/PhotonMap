#include <Cube.h>
#include <Material.h>
#include <Quad.h>

char const *Cube::type()
{
    return "Cube";
}

Cube::~Cube()
{
    for (int i = 0; i < 6; i++)
        delete faces[i];
    delete [] faces;
}

Cube::Cube(Point3D center, float scale, Material *mat)
{
    faces = new Quad*[6];
    material = mat;
    for (int i = 0; i < 4; i++)
    {
        Quad *q = new Quad(Point3D(-1,1,-1), Point3D(-1,-1,-1), Point3D(1,-1,-1), Point3D(1,1,-1), material);
        faces[i] = q;
        q->Transform(Matrix4x4::rotation(M_PI_2 * i, 'y'));
    }

    faces[4] = new Quad(Point3D(-1,1,1), Point3D(-1,1,-1), Point3D(1,1,-1), Point3D(1,1,1), material);
    faces[5] = new Quad(Point3D(-1,-1,1), Point3D(-1,-1,-1), Point3D(1,-1,-1), Point3D(1,-1,1), material);
    Transform(
                Matrix4x4::translation(Vector3D(center[0], center[1], center[2])) *
                Matrix4x4::scaling(Vector3D(scale,scale,scale)));
}

void Cube::Transform(Matrix4x4 m)
{
    for (int j = 0; j < 6; j++)
    {
        faces[j]->Transform(m);
    }
}

double Cube::intersect(Point3D o, Vector3D v, Vector3D *n)
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

void Cube::point_on_surface(Point3D &pos, Vector3D &norm)
{
	int faceIndex = rand() % 6;
	return faces[faceIndex]->point_on_surface(pos, norm);
}
