#include <Light.h>
#include <I_Scene.h>
#include <photon.h>
#include <PhotonMap.h>

PhotonMap::PhotonMap(I_Scene& scene, int num_photons)
	: num_photons_(num_photons)
	, scene_(scene)
{
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
        trace_photon(obj, 0, photonMap_);
        delete obj;
    }
}

// collide photon with the scene objects
void PhotonMap::trace_photon(photon *in_pho, int depth, vector<photon*> *out_list)
{
    // If depth >= MAX_DEPTH then Each recursive step will stop w/ a probability of 0.1
    if ((depth >= MAX_DEPTH) & (misc::RAND_2() <= 0.1))
        return;

    Point3D start_pos = in_pho->get_position();
    Vector3D direction = in_pho->get_direction();
    Color *clr = in_pho->get_color();

    Point3D i_point;
    Vector3D i_normal;
    Vector3D i_reflect;
    Vector3D i_refract;
    Color i_clr;
    Material i_mat;

    if (!trace_primary_ray(start_pos, direction, clr, &i_point, &i_normal, &i_reflect, &i_refract, &i_clr, &i_mat))
    {
        debug("miss %d: %f %f %f - %f %f %f\n", depth, start_pos[0], start_pos[1], start_pos[2], direction[0], direction[1], direction[2]);
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