#pragma once

#include <algebra.h>
#include <camera.h>
#include <I_Scene.h>
#include <I_SceneRenderer.h>
#include <misc.h>
#include <photon.h>
#include <polyroots.h>
#include <scene.h>
#include <Ray.h>
#include <shapes.h>
#include <SurfacePoint.h>
#include <vector>

class I_KdTree;
class ImagePlane;
class Light;

using namespace std;

class SceneFacade : public I_Scene, public I_SceneRenderer
{
public:
    enum RayType
    {
        Diffuse,
        Specular,
        Transmission
    };

	SceneFacade(Scene *scene, int num_samples);
	SceneFacade(int num_samples);
    ~SceneFacade();

    static SceneFacade *planeScene(int width, int height, int num_samples);

    Color *Render();
    Color *Render(vector<photon*> *photon_map);
    Color *Render(I_KdTree *kd);
	int imageWidth();
	int imageHeight();
	void emit_photons(int num_photons, vector<photon*> *photon_map);

    void Transform(Matrix4x4 m);

	Camera *cam();
	ImagePlane *imgPlane();
	std::vector<SceneObject*> *objects();
	std::vector<Light*> *lights();

private:
	Color Render(I_KdTree *kd, int x, int y);
	bool trace_ray(I_KdTree *kd, Ray ray, Color *color, int depth);
	bool trace_ray(Point3D o, Vector3D v, Color *color, int depth);
	bool trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth);
	bool trace_ray(Point3D o, Vector3D v, Color *color, Point3D *out_pos, Vector3D *out_norm, Color *out_clr, Material *out_mat, int depth);
	bool trace_primary_ray(Point3D in_pos, Vector3D in_dir, Color *in_clr, Point3D *_out_pos, Vector3D *_out_norm, Vector3D *_out_reflect, Vector3D *_out_refract, Color *_out_clr, Material *_out_mat);
	Point2D calc_image_coords(Point3D pt);

	// collide photon with the scene objects
	Color BRDF(SurfacePoint &x, Vector3D &view, Vector3D &pd);
	void initialize_photons(int num_photons, vector<photon*> *out_photons);
	RayType russian_roulette(Material *mat);
	void trace_photon(photon *in_pho, int depth, vector<photon*> *out_list);
	void bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_norm, Vector3D *i_reflect, Vector3D *i_refract, Color *i_clr, double energy, int depth, vector<photon*> *out_list);
	Color radiance_estimate(I_KdTree *kd, SurfacePoint end_pt);

	int m_samples_per_pixel;
	Camera *cam_;
	ImagePlane *imgPlane_;
	std::vector<SceneObject*> *objects_;
	std::vector<Light*> *lights_;
	Scene *scene_;
};

void BuildOrthonormalSystem(const Vector3D& v1, Vector3D& v2, Vector3D& v3);
Vector3D HemisphereSampling(Vector3D m_normal);

