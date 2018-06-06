#pragma once

#include <I_Scene.h>
#include <vector>
#include <camera.h>
#include <I_SceneRenderer.h>

class Color;
class I_KdTree;
class LightObject;
class SceneObject;
class Light;
class Material;
class photon;
class SceneFacade;

class CornellBoxScene : public I_Scene, public I_SceneRenderer
{
public:
	CornellBoxScene(int w, int h, int num_samples);
	~CornellBoxScene();

	virtual Color* Render();
    virtual Color* Render(std::vector<photon*> *photon_map);
    virtual Color *Render(I_KdTree *kd);
	virtual int imageWidth();
	virtual int imageHeight();
	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map);

	void Transform(Matrix4x4 m);

	Camera *cam();
	ImagePlane *imgPlane();
	std::vector<SceneObject*> *objects();
	std::vector<Light*> *lights();

private:
    SceneFacade *scene;

	Material *mat_ceil;
	Material *mat_grn;
	Material *mat_red;
	Material *mat_light;
	Material *mat_shiny;
	Material *mat_floor;
	LightObject *l_obj;
};
