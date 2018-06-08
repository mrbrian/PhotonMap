#pragma once

#include <algebra.h>
#include <I_SceneRenderer.h>
#include <vector>

class Camera;
class Color;
class I_KdTree;
class photon;
class I_Scene;

class PhotonSceneRenderer : public I_SceneRenderer
{
public:
	PhotonSceneRenderer(I_Scene &scene, int num_photons);
	~PhotonSceneRenderer();

    Color *Render();
	int imageWidth();
	int imageHeight();

private:
	Camera *cam();
	Point2D calc_image_coords(Point3D pt);

	I_Scene &scene_;
	int imageWidth_;
	int imageHeight_;
	std::vector<photon*> *photon_map;
	int num_photons_;
};
