#pragma once

#include <algebra.h>
#include <camera.h>
#include <I_Scene.h>
#include <polyroots.h>
#include <shapes.h>
#include <Ray.h>
#include <SurfacePoint.h>
#include <vector>

class ImagePlane;
class Light;
class SceneObject;

using namespace std;

class Scene : public I_Scene
{
public:
	Scene(
		Camera *cam, 
		ImagePlane *imgPlane,
		std::vector<Light*> *lights, 
		std::vector<SceneObject*> *objects);
	Scene(int num_samples);

    ~Scene();

    //static Scene *planeScene(int width, int height, int num_samples);

    void Transform(Matrix4x4 m);

	Camera *cam();
	ImagePlane *imgPlane();
	std::vector<SceneObject*> *objects();
	std::vector<Light*> *lights();

private:
	int samples_per_pixel_;
	Camera *cam_;
	ImagePlane *imgPlane_;
	std::vector<SceneObject*> *objects_;
	std::vector<Light*> *lights_;

};
