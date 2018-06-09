#include <Sphere.h>

char const *Sphere::type()
{
    return "Sphere";
}

void Sphere::Transform(Matrix4x4 m)
{
    center = m * center;
}

Sphere::Sphere(Point3D p, double r, Material *mat)
{
    material = mat;
    radius = r;
    center = p;
}

double Sphere::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    double t = 0;

    Vector3D D = o - center;

    double a = 1;
    double b = 2 * (v.dot(D));
    double c = D.dot(D) - (radius * radius);
    double roots[2];
    int count = quadraticRoots(a, b, c, roots);

    if (count == 0)
        return -1;

    if (count >= 1)
    {
        double minRoot = fmin(roots[0], roots[1]);

        if (minRoot < 0)
            t = fmax(roots[0], roots[1]);
        else
            t = (fmin(roots[0], roots[1]));
    }

    Vector3D &outN = *n;
    outN = (o + t * v) - center;
    outN.normalize();

    return t;
}
