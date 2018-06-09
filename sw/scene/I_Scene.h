#pragma once

#include <camera.h>
#include <vector>

class ImagePlane;
class I_SceneObject;
class Light;

class I_Scene
{
public:
	virtual ~I_Scene(){};

	virtual void Transform(Matrix4x4 m) = 0;

	virtual Camera *cam() = 0;
	virtual ImagePlane *imgPlane() = 0;
	virtual std::vector<I_SceneObject*> *objects() = 0;
	virtual std::vector<Light*> *lights() = 0;
};
