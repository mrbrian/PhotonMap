#include <Quad.h>
#include <shapes.h>
#include <Triangle.h>

char const *Quad::type()
{
    return "Quad";
}

Quad::~Quad()
{
    delete tris[0];
    delete tris[1];
    delete [] tris;
    delete [] points;
}

Quad::Quad(Point3D a, Point3D b, Point3D c, Point3D d, Material *mat)
{
    points = new Point3D[4] {a,b,c,d};
    tris = new Triangle*[2];
    tris[0] = new Triangle(a,b,c,mat);
    tris[1] = new Triangle(a,c,d,mat);

    material = mat;
}

void Quad::Transform(Matrix4x4 m)
{
    for (int j = 0; j < 2 ; j++)
    {
        tris[j]->Transform( m );
    }
}

double Quad::intersect(Point3D o, Vector3D v, Vector3D *n)
{
    for (int i = 0 ; i < 2; i++)
    {
        double t = tris[i]->intersect(o,v,n);
        if (t >= 0)
        {
            return t;
        }
    }
    return -1;
}

Point3D Quad::ImageToWorldSpace(int x, int y, int imgWidth, int imgHeight)
{
    Point3D topleft  = points[0];
    Point3D topright = points[3];
    Point3D botleft  = points[1];

    Vector3D right = topright - topleft;
    Vector3D down = topleft - botleft;

    double h_pct = (float)x / imgWidth;
    double v_pct = (float)y / imgHeight;

    Point3D result = (topleft + (h_pct * right) + (v_pct * down));
    return result;
}

void Quad::point_on_surface(Point3D &pos, Vector3D &normal)
{
    // return a random point on surface
    Point3D topleft  = points[0];
    Point3D topright = points[1];
    Point3D botleft  = points[3];

    Vector3D right = topright - topleft;
    Vector3D down = botleft - topleft;

    double h_pct = misc::RAND_2();
    double v_pct = misc::RAND_2();

    pos = (topleft + (h_pct * right) + (v_pct * down));
    normal = tris[0]->normal();
}
