#include <I_Scene.h>
#include <vector>

class Color;
class I_KdTree;
class LightObject;
class Material;
class photon;
class Scene;

class CornellBoxScene : public I_Scene
{
public:
	CornellBoxScene(int w, int h);
	~CornellBoxScene();

	virtual Color* Render();
    virtual Color* Render(std::vector<photon*> *photon_map);
    virtual Color *Render(I_KdTree *kd);
	virtual int imageWidth();
	virtual int imageHeight();
	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map);

private:
    Scene *scene;

	Material *mat_ceil;
	Material *mat_grn;
	Material *mat_red;
	Material *mat_light;
	Material *mat_shiny;
	Material *mat_floor;
	LightObject *l_obj;
};
