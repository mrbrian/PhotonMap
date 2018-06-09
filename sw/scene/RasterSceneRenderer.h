#pragma once

#include <algebra.h>
#include <I_SceneRenderer.h>
#include <vector>

class Camera;
class Color;
class I_KdTree;
class I_Scene;
class I_SceneObject;
class ImagePlane;
class Light;
class photon;

class RasterSceneRenderer : public I_SceneRenderer
{
public:
	RasterSceneRenderer(I_Scene &scene);
	~RasterSceneRenderer();

    Color *Render();
	int imageWidth();
	int imageHeight();

private:
	Camera *cam();
	ImagePlane *imagePlane();
	bool trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth);
	bool trace_ray(Point3D o, Vector3D v, Color *color, int depth);
	std::vector<I_SceneObject*> *objects();
	std::vector<Light*> *lights();
	bool intersection_test(I_Scene *scene, Point3D o, Point3D light_pos);

	I_Scene &scene_;
	int imageWidth_;
	int imageHeight_;

    int MAX_DEPTH;
    Color BG_COLOR;
    double NORM_EPSILON;
};
