#pragma once

#include <algebra.h>

class Material;

class SurfacePoint
{
public:
	Point3D position;
	Vector3D normal;
	Material *material;

	SurfacePoint() {}

	SurfacePoint(Point3D p, Vector3D v, Material *m)
	{
		position = p;
		normal = v;
		material = m;
	}
};
