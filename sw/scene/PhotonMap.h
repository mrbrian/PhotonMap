#pragma once

#include <I_PhotonMap.h>
#include <vector>
#include <Ray.h>

class I_Scene;
class I_SceneObject;
class Light;
class Material;
class photon;

class PhotonMap : public I_PhotonMap
{
public:
	PhotonMap(I_Scene& scene, int num_photons);
	virtual ~PhotonMap();

	virtual const photon &getPhoton(int i) const;
	virtual int count() const;

private:
	void emit_photons();
	void initialize_photons(std::vector<photon*> &out_photons);
	std::vector<Light*> *lights();
	void trace_photon(photon *in_pho, int depth, std::vector<const photon*> *out_list);
	I_SceneObject *find_closest_intersection(Point3D o, Vector3D v, double *t_min_ptr, Vector3D *n_min_ptr);
		RayType russian_roulette(Material *mat);
	void bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_normal, Vector3D *i_reflect,
		Vector3D *i_refract, Color *i_clr, double energy, int depth,
		std::vector<const photon*> *out_list);
	bool trace_primary_ray(
		Point3D in_pos,
		Vector3D in_dir,
		Color *in_clr,
		Point3D *_out_pos,
		Vector3D *_out_norm,
		Vector3D *_out_reflect,
		Vector3D *_out_refract,
		Color *_out_clr,
		Material *_out_mat);
	Vector3D HemisphereSampling(Vector3D m_normal);
	void BuildOrthonormalSystem(const Vector3D& v1, Vector3D& v2, Vector3D& v3);

	int MAX_DEPTH;
	Color BG_COLOR;
	double NORM_EPSILON;
	int num_photons_;
	I_Scene &scene_;
	std::vector<const photon*> photonMap_;
};