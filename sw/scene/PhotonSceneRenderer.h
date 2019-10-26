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
	PhotonSceneRenderer(I_Scene &scene, I_PhotonMap &photonMap);
	~PhotonSceneRenderer();

    Color *Render();
	I_PhotonMap &photonMap();
	Point2D calc_image_coords(Point3D pt);

private:
	Camera *cam();

	I_Scene &scene_;
	int imageWidth_;
	int imageHeight_;
	I_PhotonMap &photonMap_;
};
