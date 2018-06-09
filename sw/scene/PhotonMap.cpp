#include <misc.h>
#include <Light.h>
#include <I_SceneObject.h>
#include <I_Scene.h>
#include <photon.h>
#include <PhotonMap.h>
#include <Material.h>
#include <Ray.h>

PhotonMap::PhotonMap(I_Scene& scene, int num_photons)
	: num_photons_(num_photons)
	, scene_(scene)
{
	MAX_DEPTH = 5;
	BG_COLOR = Color(1, 0, 0);
	NORM_EPSILON = 0.001;
	emit_photons();
}

const photon& PhotonMap::getPhoton(int i) const
{
	return *photonMap_.at(i);
}

int PhotonMap::count() const
{
	return photonMap_.size();
}

void PhotonMap::emit_photons()
{
    vector<photon*> initialPhotons;

    initialize_photons(initialPhotons);    // spawn initial photons for all lights

    for(std::vector<photon*>::iterator it = initialPhotons.begin(); it != initialPhotons.end(); ++it)
    {
        photon *obj = (*it);
        trace_photon(obj, 0, &photonMap_);
        delete obj;
    }
}

// collide photon with the scene objects
void PhotonMap::trace_photon(photon *in_pho, int depth, vector<const photon*> *out_list)
{
    // If depth >= MAX_DEPTH then Each recursive step will stop w/ a probability of 0.1
    if ((depth >= MAX_DEPTH) & (misc::RAND_2() <= 0.1))
        return;

    Point3D start_pos = in_pho->get_position();
    Vector3D direction = in_pho->get_direction();
    Color clr = in_pho->get_color();

    Point3D i_point;
    Vector3D i_normal;
    Vector3D i_reflect;
    Vector3D i_refract;
    Color i_clr;
    Material i_mat;

    if (!trace_primary_ray(start_pos, direction, &clr, &i_point, &i_normal, &i_reflect, &i_refract, &i_clr, &i_mat))
    {
        //debug("miss %d: %f %f %f - %f %f %f\n", depth, start_pos[0], start_pos[1], start_pos[2], direction[0], direction[1], direction[2]);
        return;
    }

    RayType ray_type = russian_roulette(&i_mat);

    if (ray_type == RayType::Diffuse)
        i_clr = Color(i_clr.R() * i_mat.Kd.R(), i_clr.G() * i_mat.Kd.G(), i_clr.B() * i_mat.Kd.B());

    // debug("%f %f %f\n",
    //        i_normal.get_x(),
    //        i_normal.get_y(),
    //        i_normal.get_z()
    //     );
    // debug("%d: %f %f %f - %f %f %f - %f %f %f\n", depth, (start_pos)[0], (start_pos)[1], (start_pos)[2],
    //         direction[0], direction[1], direction[2],
    //         i_clr.R(), i_clr.G(), i_clr.B()
    //         );

    // store photon in photon list
    photon *store_photon = new photon(i_point, direction, i_clr, in_pho->power);

    if (i_clr.R() <= 0.01 &&
        i_clr.G() <= 0.01 &&
        i_clr.B() <= 0.01)
    {
        delete store_photon;
        return;
    }
    out_list->push_back(store_photon);

    bounce_photon(ray_type, &i_point, &i_normal, &i_reflect, &i_refract, &i_clr, store_photon->power, depth, out_list);
}

bool PhotonMap::trace_primary_ray(Point3D in_pos, Vector3D in_dir, Color *in_clr, Point3D *_out_pos, Vector3D *_out_norm, Vector3D *_out_reflect, Vector3D *_out_refract, Color *_out_clr, Material *_out_mat)
{
	Point3D &out_pos = *_out_pos;
	Vector3D &out_norm = *_out_norm;
	Vector3D &out_reflect = *_out_reflect;
	Vector3D &out_refract = *_out_refract;
	Color &out_clr = *_out_clr;
	Material &out_mat = *_out_mat;

	double t_min = INFINITY;
	Vector3D n_min;
	I_SceneObject *hitObject = NULL;
	in_dir.normalize();

	// find closest intersection point, the object
	hitObject = find_closest_intersection(in_pos, in_dir, &t_min, &n_min);

	if (hitObject == NULL)              // check for no intersection
		return false;

	out_mat = *hitObject->material;

	out_pos = in_pos + t_min * in_dir;      // calculate intersection point
											//p_int = p_int + 0.001 * n_min;      // pull back point a bit, avoid self intersection

	out_norm = n_min;
	out_reflect = in_dir + (2 * n_min.dot(-in_dir)) * n_min;

	out_clr = *in_clr;
	// add diffuse color
	//out_clr = (hitObject->material->GetKd(p_int));//* fmax(n.dot(in_dir), 0) * light->Id);
	return true;
}

I_SceneObject *PhotonMap::find_closest_intersection(Point3D o, Vector3D v, double *t_min_ptr, Vector3D *n_min_ptr)
{
	I_SceneObject *hitObject = NULL;
	double &t_min = *t_min_ptr;
	Vector3D &n_min = *n_min_ptr;

	for (std::vector<I_SceneObject*>::iterator it = scene_.objects()->begin(); it != scene_.objects()->end(); ++it)
	{
		I_SceneObject *obj = (*it);

		Vector3D n;
		double t = obj->intersect(o, v, &n);      // whats the n?

		if (0 <= t && t < t_min)
		{
			t_min = t;
			if (n.dot(v) >= 0)
				n = -n;
			n_min = n;
			hitObject = obj;
		}
	}
	return hitObject;
}

void PhotonMap::initialize_photons(std::vector<photon*> &out_photons)
{
    double total_watts = 0;
    double energy_per_photon = 0;

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
    {
        Light *obj = (*it);
        total_watts += obj->watts;
    }

    energy_per_photon = total_watts / num_photons_;

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
    {
        Light *obj = (*it);
        int num_to_emit = (int)(obj->watts / energy_per_photon);
        obj->emit_photons(num_to_emit, energy_per_photon, &out_photons);
    }
}

std::vector<Light*> *PhotonMap::lights()
{
	return scene_.lights();
}

RayType PhotonMap::russian_roulette(Material *mat)   // [0, d] diffuse reflection ray, [d,s+d] specular ray, [s+d, 1] absorption
{
	Color diff = mat->Kd;
	Color spec = mat->Ks;

	double p_refl = std::max(diff.R() + spec.R(), diff.G() + spec.G());
	p_refl = std::max(p_refl, diff.B() + spec.B());

	float diff_sum = diff.R() + diff.G() + diff.B();
	float spec_sum = spec.R() + spec.G() + spec.B();

	float p_diff = diff_sum / (diff_sum + spec_sum) * p_refl;
	float p_spec = spec_sum / (diff_sum + spec_sum) * p_refl;

	float r = misc::RAND_2();

	if (r >= 0 && r < p_diff)
		return RayType::Diffuse;
	if (r >= p_diff && r < p_spec)
		return RayType::Specular;
	return RayType::Transmission;
}

void PhotonMap::bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_normal, Vector3D *i_reflect,
	Vector3D *i_refract, Color *i_clr, double energy, int depth, vector<const photon*> *out_list)
{
	photon *new_photon;
	Vector3D new_dir;
	Point3D new_pos;
	if (i_clr->R() <= 0.01 &&
		i_clr->G() <= 0.01 &&
		i_clr->B() <= 0.01)
		return;

	switch (ray_type)
	{
	case Diffuse:
		//  choose random hemisphere direction
		new_pos = (*i_pos) + NORM_EPSILON * (*i_normal);      // pull back point a bit, avoid self intersection
		new_dir = HemisphereSampling(*i_normal);
		break;
	case Specular:
		// choose a reflect direction
		// calculate reflect vector
		new_pos = (*i_pos) + NORM_EPSILON * (*i_normal);      // pull back point a bit, avoid self intersection
		new_dir = *i_reflect;

		break;
	case Transmission:
		// check if transparent
		// if not then nothin
		new_pos = (*i_pos) - NORM_EPSILON * (*i_normal);      // pull back point a bit, avoid self intersection
		new_dir = *i_refract;
		return;
	}
	new_photon = new photon(new_pos, new_dir, *i_clr, energy);

	trace_photon(new_photon, depth + 1, out_list);

	delete new_photon;
}


// ----------------------------------------------------------------------------
// HemisphereSampling ()
//
// Generating outgoing ray directions by uniform sampling on a hemisphere
//
// Input: normal vector at the intersection point
// Output: outgoing ray direction from uniform sampling on a hemisphere
//
// ----------------------------------------------------------------------------

Vector3D PhotonMap::HemisphereSampling(Vector3D m_normal)
{
	float r_1 = misc::RAND_2();
	float r_2 = misc::RAND_2();

	float r = sqrt(1 - r_1 * r_1);
	float phi = 2 * M_PI * r_2;

	double vx = cos(phi) * r;
	double vy = sin(phi) * r;
	double vz = r_1;

	Vector3D sampled_ray_direction = Vector3D(vx, vy, vz);

	// Now we build an otrhotnormal frame system
	// to "rotate" the sampled ray to this system
	Vector3D v2, v3;
	BuildOrthonormalSystem(m_normal, v2, v3);

	// Construct the normalized rotated vector
	double vecx = Vector3D(v2[0], v3[0], m_normal[0]).dot(sampled_ray_direction);
	double vecy = Vector3D(v2[1], v3[1], m_normal[1]).dot(sampled_ray_direction);
	double vecz = Vector3D(v2[2], v3[2], m_normal[2]).dot(sampled_ray_direction);
	Vector3D m_rotated_ray_direction = Vector3D(vecx, vecy, vecz);

	m_rotated_ray_direction.normalize();
	return m_rotated_ray_direction;
}

// ----------------------------------------------------------------------------
// BuildOrthonormalSystem ()
//
// Generating outgoing ray directions by uniform sampling on a hemisphere
//
// Input: vectors v1 and v2
// Output: vector v3 so (v1, v2, v3) form an orthonormal system
//         (assuming v1 is already normalized)
//
// ----------------------------------------------------------------------------
void PhotonMap::BuildOrthonormalSystem(const Vector3D& v1, Vector3D& v2, Vector3D& v3)
{
	float inverse_length, den;

	if (std::abs(v1[0]) > std::abs(v1[1]))
	{
		// project to the plane y = 0
		// construct a normalized orthogonal vector on this plane
		den = sqrtf(v1[0] * v1[0] + v1[2] * v1[2]);
		inverse_length = 1.f / den;
		v2 = Vector3D(-v1[2] * inverse_length, 0.0f, v1[0] * inverse_length);
	}
	else
	{
		// project to the plane x = 0
		// construct a normalized orthogonal vector on this plane
		den = sqrtf(v1[1] * v1[1] + v1[2] * v1[2]);
		inverse_length = 1.0f / den;
		v2 = Vector3D(0.0f, v1[2] * inverse_length, -v1[1] * inverse_length);
	}

	// construct v3 as the cross-product between v1 and v2
	v3 = v1.cross(v2);
}