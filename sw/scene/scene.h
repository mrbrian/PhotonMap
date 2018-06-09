#pragma once

#include <algebra.h>
#include <camera.h>
#include <I_Scene.h>
#include <polyroots.h>
#include <Ray.h>
#include <SurfacePoint.h>
#include <vector>

class ImagePlane;
class Light;
class I_SceneObject;

using namespace std;

class Scene : public I_Scene
{
public:
	Scene(
		Camera *cam,
		ImagePlane *imgPlane,
		std::vector<Light*> *lights,
		std::vector<I_SceneObject*> *objects);
	Scene(int num_samples);

    ~Scene();

    //static Scene *planeScene(int width, int height, int num_samples);

    void Transform(Matrix4x4 m);

	Camera *cam();
	ImagePlane *imgPlane();
	std::vector<I_SceneObject*> *objects();
	std::vector<Light*> *lights();

private:
	int samples_per_pixel_;
	Camera *cam_;
	ImagePlane *imgPlane_;
	std::vector<I_SceneObject*> *objects_;
	std::vector<Light*> *lights_;

};
