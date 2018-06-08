#pragma once

#include <algebra.h>
#include <I_SceneRenderer.h>
#include <vector>

class Camera;
class Color;
class I_KdTree;
class I_PhotonMap;
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
	I_PhotonMap &photonMap();
private:
	Camera *cam();
	Point2D calc_image_coords(Point3D pt);

	I_Scene &scene_;
	int imageWidth_;
	int imageHeight_;
	int num_photons_;
	I_PhotonMap *photonMap_;
};
