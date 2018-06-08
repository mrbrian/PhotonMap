#pragma once

#include <algebra.h>
#include <I_SceneRenderer.h>
#include <Ray.h>
#include <SurfacePoint.h>
#include <vector>

class Camera;
class Color;
class I_KdTree;
class I_Scene;
class ImagePlane;
class Light;
class photon;
class SceneObject;

class PhotonMappingSceneRenderer : public I_SceneRenderer
{
public:
	PhotonMappingSceneRenderer(I_Scene &scene, int samples, int num_photons);
	~PhotonMappingSceneRenderer();

    Color *Render();
	int imageWidth();
	int imageHeight();

private:
	Color radiance_estimate(I_KdTree *kd, SurfacePoint end_pt);
	Color Render(I_KdTree *kd, int x, int y);
	Color *Render(I_KdTree *kd);
	Camera *cam();
	ImagePlane *imagePlane();
	std::vector<SceneObject*> *objects();
	std::vector<Light*> *lights();
	Color BRDF(SurfacePoint &x, Vector3D& view, Vector3D& pd);

	bool trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth);
	bool trace_ray(I_KdTree *kd, Ray ray, Color *color, int depth);

	I_Scene &scene_;
	int imageWidth_;
	int imageHeight_;
	I_KdTree *kd;

    int MAX_DEPTH;
    Color BG_COLOR;
    double NORM_EPSILON;
    int samples_per_pixel_;
};
