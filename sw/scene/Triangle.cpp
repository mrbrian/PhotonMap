#include <misc.h>
#include <Triangle.h>

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
    normal_ = -(verts[1] - verts[0]).cross(verts[2] - verts[0]);
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

void Triangle::point_on_surface(Point3D &pos, Vector3D &norm)
{
	Vector3D v1 = verts[1] - verts[0];
	Vector3D v2 = verts[2] - verts[0];
	double factor = misc::RAND_2();
	pos = verts[0] +
		factor * v1 +
		(1 - factor) * v2;
	
	norm = normal_;
}

Vector3D &Triangle::normal() const
{
	return Vector3D(normal_);
}
