#pragma once

#include <algebra.h>
#include <I_SceneRenderer.h>
#include <Ray.h>
#include <SurfacePoint.h>
#include <vector>

class Camera;
class Color;
class I_KdTree;
class I_PhotonMap;
class I_Scene;
class ImagePlane;
class Light;
class photon;
class I_SceneObject;

class PhotonMappingSceneRenderer : public I_SceneRenderer
{
public:
	PhotonMappingSceneRenderer(I_Scene &scene, I_KdTree &kd, int num_samples, 
		int width, int height);
	~PhotonMappingSceneRenderer();

    Color *Render();
	int imageWidth();
	int imageHeight();

private:
	Color radiance_estimate(I_KdTree *kd, SurfacePoint end_pt);
	Color Render(I_KdTree *kd, int x, int y);
	Camera *cam();
	ImagePlane *imagePlane();
	std::vector<I_SceneObject*> *objects();
	std::vector<Light*> *lights();
	Color BRDF(SurfacePoint &x, Vector3D& view, Vector3D& pd);

	bool trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth);
	bool trace_ray(I_KdTree *kd, Ray ray, Color *color, int depth);

	I_Scene &scene_;
	I_KdTree &kd_;
	int imageWidth_;
	int imageHeight_;
    int MAX_DEPTH;
    Color BG_COLOR;
    double NORM_EPSILON;
    int samples_per_pixel_;
};
