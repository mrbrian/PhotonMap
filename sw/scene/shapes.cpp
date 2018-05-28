#include <quad.h>
#include <shapes.h>

extern double RAND_2();

char const *Triangle::type()
{
    return "Triangle";
}

Triangle::Triangle(Point3D p1, Point3D p2, Point3D p3, Material *mat)
{
    material = mat;
    verts[0] = p1;
    verts[1] = p2;
    verts[2] = p3;
    normal = -(verts[1] - verts[0]).cross(verts[2] - verts[0]);
}

double Triangle::area(Point3D a, Point3D b, Point3D c)
{
    Vector3D ab = b - a;
    Vector3D ac = c - a;
    double area2 = 0.5 * ab.cross(ac).length();
    return area2;
}

double Triangle::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    v.normalize();
    Vector3D &tri_normal = *n;
    tri_normal = (verts[1] - verts[0]).cross(verts[2] - verts[0]);
    tri_normal.normalize();

    Point3D tri_pt;
    tri_pt[0] = (verts[0][0] + verts[1][0] + verts[2][0]) / 3;
    tri_pt[1] = (verts[0][1] + verts[1][1] + verts[2][1]) / 3;
    tri_pt[2] = (verts[0][2] + verts[1][2] + verts[2][2]) / 3;

    Vector3D delta = o - tri_pt;
    double t = -delta.dot(tri_normal)/(v.dot(tri_normal));
    Point3D p = o + t * v;

    double total_area = 0.5 * ((verts[0] - verts[1]).cross(verts[0] - verts[2])).length();

    double s1 = 0.5 * ((verts[2] - verts[1]).cross(p - verts[1])).dot(tri_normal);
    double s2 = 0.5 * ((verts[0] - verts[2]).cross(p - verts[2])).dot(tri_normal);

    double alpha = s1 / total_area;
    double beta  = s2 / total_area;
    double y     = 1 - alpha - beta;

    if (alpha < 0 || alpha > 1)
        t = -1;
    if (beta < 0 || beta > 1)
        t = -1;
    if (y < 0 || y > 1)
        t = -1;

    return t;
}

void Triangle::Transform(Matrix4x4 m)
{
    for (int i = 0; i < 3 ;i ++)
    {
        verts[i] = m * verts[i];
    }
}

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

char const *Cube::type()
{
    return "Cube";
}

Cube::~Cube() {
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
