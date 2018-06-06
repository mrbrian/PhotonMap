#pragma once

#include <algebra.h>

struct Ray
{
public:
	Point3D origin;
	Vector3D direction;

	Ray(Point3D p, Vector3D v)
	{
		origin = p;
		direction = v;
	}
};
