#pragma once

#include <I_Scene.h>
#include <vector>
#include <camera.h>

class Color;
class I_KdTree;
class LightObject;
class I_SceneObject;
class Light;
class Material;
class photon;

class CornellBoxScene : public I_Scene
{
public:
	CornellBoxScene(int w, int h);
	~CornellBoxScene();
	void Transform(Matrix4x4 m);

	Camera *cam();
	ImagePlane *imgPlane();
	std::vector<I_SceneObject*> *objects();
	std::vector<Light*> *lights();

private:
    I_Scene *scene;

	Material *mat_ceil;
	Material *mat_grn;
	Material *mat_red;
	Material *mat_light;
	Material *mat_shiny;
	Material *mat_floor;
	LightObject *l_obj;
};
