#include <Material.h>

#define DEF_REFLECT     Color(0.1, 0.1, 0.1)

// procedural texture
Color CheckerMaterial::GetKd(Point3D p)
{
    if (((int)(round(p[0] / squareSize) + round(p[1] / squareSize) + round(p[2] / squareSize)) % 2) == 0)
        return Color(1,1,1) * Kd;
    else
        return Color(0,0,0);
}

CheckerMaterial::CheckerMaterial(Color a, Color d, Color s, double in_p, Color r, float sc)
    : Material(a, d, s, in_p, r)
{
    squareSize = sc;
}

Material::Material(Color a, Color d, Color s, double in_p, Color r)
{
    Kr = r;
    Ka = a;
    Kd = d;
    Ks = s;
    p = in_p;
}

Material::Material(Color a, Color d, Color s, double in_p)
{
    Kr = DEF_REFLECT;
    Ka = a;
    Kd = d;
    Ks = s;
    p = in_p;
}
