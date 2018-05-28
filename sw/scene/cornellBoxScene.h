#include <I_Scene.h>
#include <vector>

class photon;
class Scene;

class CornellBoxScene : public I_Scene
{
public:
	CornellBoxScene(int w, int h);
	~CornellBoxScene();

	virtual void emit_photons(int num_photons, std::vector<photon*> *photon_map);

private:
	void delete_objects();
    Scene *scene;
};
