#include <I_KdTree.h>
#include <imagePlane.h>
#include <Light.h>
#include <LightObject.h>
#include <quad.h>
#include <SceneFacade.h>

//#define DEBUG 1
//#define debugging_enabled 1

#ifdef DEBUG
#define debug(fmt, ...)  do { \
  if (debugging_enabled) { printf(fmt, ##__VA_ARGS__); } \
} while (0)
#else
#define debug(fmt, ...)
#endif

#define MAX_DEPTH   5
#define BG_COLOR    Color(1,0,0)
#define NORM_EPSILON     0.001

SceneFacade::SceneFacade(Scene *scene, int num_samples)
	: scene_(scene)
    , m_samples_per_pixel(num_samples)
{
}

SceneFacade::SceneFacade(int num_samples)
    : m_samples_per_pixel(num_samples)
{
}

SceneFacade::~SceneFacade()
{
    for (std::vector<SceneObject* >::iterator it = objects_->begin() ; it != objects_->end(); ++it)
    {
        delete (*it);
    }
    objects_->clear();
    delete imgPlane_;
}

Color *SceneFacade::Render()
{
    Color * result = new Color[cam()->imgWidth * cam()->imgHeight];
    // iterate over the pixels & set colour values
    for (int x = 0; x < cam()->imgWidth; x++)
    {
        for (int y = 0; y < cam()->imgHeight; y++)
        {
            // determine ray vector
            Point3D p = imgPlane_->ImageToWorldSpace(x, y, cam()->imgWidth, cam()->imgHeight);
            Vector3D v = p - cam()->position;
            v.normalize();

            Color &c = result[x + y * cam()->imgWidth];

            if (!trace_ray_lights(cam()->position, v, &c, 1))   // if ray hit nothing
                if (!trace_ray(cam()->position, v, &c, 1))   // if ray hit nothing
                    c = BG_COLOR;                               // use background color
        }
    }
    return result;
}

Point2D SceneFacade::calc_image_coords(Point3D pt)
{
    Matrix4x4 m_view = cam()->get_view_matrix();
    Matrix4x4 *m_projection = cam()->calc_proj_matrix();

    float width = cam()->imgWidth;
    float height = cam()->imgHeight;

    Matrix4x4 m_screenCoords = Matrix4x4();           // make transform for converting NDC space to screenspace
    m_screenCoords[0][0] = -width / 2;
    m_screenCoords[1][1] = height / 2;
    m_screenCoords = Matrix4x4::translation(Vector3D(width / 2, height / 2, 0)) * m_screenCoords;

    // Apply the view matrix
    pt = m_view * pt;

    // Do clipping here...
    //bool skipLine = false;
    //Point3D pt_i;

    //do_clip();

    // Apply the projection matrix to 4D points
    Point4D a_4d = Point4D(pt);

    a_4d = (*m_projection) * a_4d;

    // homogenization & store back into 3D points
    pt = Point3D(a_4d[0] / a_4d[3], a_4d[1] / a_4d[3], a_4d[2] / a_4d[3]);

    // map to viewport
    pt = m_screenCoords * pt;

    //delete(m_view);
    delete(m_projection);

    return Point2D(pt[0], pt[1]);
}

Color *SceneFacade::Render(vector<photon*> *photon_map)
{
    Color * result = new Color[cam()->imgWidth * cam()->imgHeight];
    // iterate over the pixels & set colour values

    for (int x = 0; x < cam()->imgWidth; x++)
    {
        for (int y = 0; y < cam()->imgHeight; y++)
        {
            result[x + y * cam()->imgWidth] = Color(0, 0, 0);  // initialize to all black
        }
    }

    for(std::vector<photon*>::iterator it = photon_map->begin(); it != photon_map->end(); ++it)
    {
        photon *p = (*it);

        // calc image space coordinates
        Point2D img_coords = calc_image_coords(p->get_position());
        int x = (int)img_coords[0];
        int y = (int)img_coords[1];
        int idx = x + y * cam()->imgWidth;
        if (idx < 0 || idx >= cam()->imgHeight * cam()->imgWidth)
            continue;
        Color &pixel = result[x + y * cam()->imgWidth];
        pixel = pixel + *(p->get_color());
    }

    return result;
}

// check if objects lay between a point and light position
bool intersection_test(SceneFacade *scene, Point3D o, Point3D light_pos)
{
    Vector3D v = light_pos - o;
    double dist = v.length();
    v.normalize();

    float tmin = INFINITY;

    for(std::vector<SceneObject*>::iterator it = scene->objects()->begin(); it != scene->objects()->end(); ++it)
    {
        SceneObject *object= (*it);

        Vector3D n;
        double t = object->intersect(o, v, &n);      // whats the n?    -1 if no hit.

        if (0 <= t && t <= tmin)
            tmin = t;
    }
    if (tmin > dist)
        return false;
    if (tmin < 0)
        return false;

    return true;
}

bool SceneFacade::trace_ray(Point3D o, Vector3D v, Color *color, int depth)
{
    if (depth > MAX_DEPTH)  // stop recursing
        return false;

    double t_min = INFINITY;
    Vector3D n_min;
    SceneObject *hitObject = NULL;
    v.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<SceneObject*>::iterator it = objects_->begin(); it != objects_->end(); ++it)
    {
        SceneObject *obj = (*it);

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

    if (hitObject == NULL)              // check for no intersection
    {
        return false;
    }

    Point3D p_int = o + t_min * v;      // calculate intersection point

    p_int = p_int + 0.001 * n_min;      // pull back point a bit, avoid self intersection

    // found closest intersection
    Point3D p = o + t_min * v;

    for(std::vector<Light*>::iterator it = lights_->begin(); it != lights_->end(); ++it)
    {
        Light *light= (*it);
        col = col + hitObject->material->Ka;

        Point3D new_p = p + 0.1 * (light->position - p);
        if (intersection_test(this, new_p, light->position))
           continue;

        Vector3D n = n_min;
        Vector3D l = light->position - p;
        l.normalize();
        Vector3D r = -l + 2 * (l.dot(n)) * n;

        // add diffuse color
        col = col + (hitObject->material->GetKd(p) * fmax(n.dot(l), 0) * light->clr);
        // add specular color
        col = col + (hitObject->material->Ks * pow(fmax(r.dot(-v), 0), hitObject->material->p) * light->clr);
    }

    Color reflect_rgb;     // reflection color;

    // calculate reflect vector
    Vector3D r = v + (2 * n_min.dot(-v)) * n_min;

    trace_ray(p_int, r, &reflect_rgb, depth + 1);
    // add reflection color
    col = col + reflect_rgb * hitObject->material->Kr;
    return true;
}


SceneObject *find_closest_intersection(SceneFacade *scene, Point3D o, Vector3D v, double *t_min_ptr, Vector3D *n_min_ptr)
{
    SceneObject *hitObject = NULL;
    double &t_min = *t_min_ptr;
    Vector3D &n_min = *n_min_ptr;

    for (std::vector<SceneObject*>::iterator it = scene->objects()->begin(); it != scene->objects()->end(); ++it)
    {
        SceneObject *obj = (*it);

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

bool SceneFacade::trace_primary_ray(Point3D in_pos, Vector3D in_dir, Color *in_clr, Point3D *_out_pos, Vector3D *_out_norm, Vector3D *_out_reflect, Vector3D *_out_refract, Color *_out_clr, Material *_out_mat)
{
    Point3D &out_pos = *_out_pos;
    Vector3D &out_norm = *_out_norm;
    Vector3D &out_reflect = *_out_reflect;
    Vector3D &out_refract = *_out_refract;
    Color &out_clr = *_out_clr;
    Material &out_mat = *_out_mat;

    double t_min = INFINITY;
    Vector3D n_min;
    SceneObject *hitObject = NULL;
    in_dir.normalize();

    // find closest intersection point, the object
    hitObject = find_closest_intersection(this, in_pos, in_dir, &t_min, &n_min);

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


// collide photon with the scene objects
void SceneFacade::trace_photon(photon *in_pho, int depth, vector<photon*> *out_list)
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

    debug("%f %f %f\n",
           i_normal.get_x(),
           i_normal.get_y(),
           i_normal.get_z()
        );
    debug("%d: %f %f %f - %f %f %f - %f %f %f\n", depth, (start_pos)[0], (start_pos)[1], (start_pos)[2],
            direction[0], direction[1], direction[2],
            i_clr.R(), i_clr.G(), i_clr.B()
            );

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

void SceneFacade::emit_photons(int num_photons, vector<photon*> *photon_map)
{
    vector<photon*> photons;

    initialize_photons(num_photons, &photons);    // spawn initial photons for all lights

    for(std::vector<photon*>::iterator it = photons.begin(); it != photons.end(); ++it)
    {
        photon *obj = (*it);
        trace_photon(obj, 0, photon_map);
        delete obj;
    }
}

void SceneFacade::bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_normal, Vector3D *i_reflect,
                          Vector3D *i_refract, Color *i_clr, double energy, int depth, vector<photon*> *out_list)
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

SceneFacade::RayType SceneFacade::russian_roulette(Material *mat)   // [0, d] diffuse reflection ray, [d,s+d] specular ray, [s+d, 1] absorption
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

void SceneFacade::initialize_photons(int num_photons, vector<photon*> *out_photons)
{
    double total_watts = 0;
    double energy_per_photon = 0;

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
    {
        Light *obj = (*it);
        total_watts += obj->watts;
    }

    energy_per_photon = total_watts / num_photons;

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
    {
        Light *obj = (*it);
        int num_to_emit = (int)(obj->watts / energy_per_photon);
        obj->emit_photons(num_to_emit, energy_per_photon, out_photons);
    }
}

// phong brdf
// x = surfact point
// pd = incident direction
// v = reflected direction

Color SceneFacade::BRDF(SurfacePoint &x, Vector3D& view, Vector3D& pd)
{
    Vector3D ref_dir = pd + (2 * x.normal.dot(-pd)) * x.normal;

    double pd_dot_norm = std::max(0.0, -pd.dot(x.normal));
    double ref_dot_view = std::max(0.0, -ref_dir.dot(view));

    Color diff = pd_dot_norm * x.material->Kd;
    Color spec = ref_dot_view * x.material->Ks;

    return (diff + spec);
}

Color SceneFacade::radiance_estimate(I_KdTree *kd, SurfacePoint end_pt)
{
    // how much light is at this point?
    // locate k nearest photons
    // how much light from each

    const photon p(end_pt.position, Vector3D(0,1,0), Color(0,0,0), 0);
    std::vector<photon> nearest = kd->getKNearest(p, m_samples_per_pixel);

    int num_photons = nearest.size();

    if (num_photons == 0)
        return Color(-1,-1,-1);

    double r_2 = 0;  //distance to kth nearest photon
    Color flux = Color(0,0,0);

//    debug("%f %f %f - %f %f %f\n",
//           end_pt.normal.get_x(),
//           end_pt.normal.get_y(),
//           end_pt.normal.get_z(),
//           end_pt.position[0],
//           end_pt.position[1],
//           end_pt.position[2]
//           );
    for ( std::vector<photon>::iterator it = nearest.begin(); it != nearest.end(); ++it)
    {
        photon pho = *it;
        Vector3D delta = pho.get_position() - end_pt.position;
        if (delta.length2() > r_2)
            r_2 = delta.length2();

//        debug("%f %f %f %f - %f %f %f - %f %f %f \n",
//               pho.get_color()->R(),
//               pho.get_color()->G(),
//               pho.get_color()->B(),
//               delta.length2(),
//               pho.get_direction().get_x(),
//               pho.get_direction().get_y(),
//               pho.get_direction().get_z(),
//               pho.get_position()[0],
//               pho.get_position()[1],
//               pho.get_position()[2]
//               );

        Vector3D pd = pho.get_direction();
        double pw = pho.power;

        Vector3D eye_dir = pho.get_direction() - cam()->position; // eye direction

        Color brdf = BRDF(end_pt, eye_dir, pd);
        flux = flux + brdf * pw * pho.color;
    }

//    debug("radius2 %f\n", r_2);
    flux = flux / (M_PI * r_2);
    return flux;
}

Color SceneFacade::Render(I_KdTree *kd, int x, int y)
{
    // iterate over the pixels & set colour values
    // determine ray vector
    Point3D p = imgPlane_->ImageToWorldSpace(x, y, cam()->imgWidth, cam()->imgHeight);
    Vector3D v = p - cam()->position;
    v.normalize();

    Color c;
    Ray ray = Ray(p, v);

    if (!trace_ray_lights(ray.origin, ray.direction, &c, 1))   // if ray hit a light
    {
        if (!trace_ray(kd, ray, &c, 1))   // if ray hit nothing
            c = BG_COLOR;                       // use background color
    }
    return c;
}

Color *SceneFacade::Render(I_KdTree *kd)
{
    Color * result = new Color[cam()->imgWidth * cam()->imgHeight];
    // iterate over the pixels & set colour values
    //#pragma omp parallel for schedule(dynamic)

    for (int x = 0; x < cam()->imgWidth; x++)
    {
        float perc = (float) x / cam()->imgWidth * 100;
        fprintf(stdout,"\rRendering: %d Samples per Pixel %8.2f%%", 5, (double)perc);

        for (int y = 0; y < cam()->imgHeight; y++)
        {

            result[x + y * cam()->imgWidth] = Render(kd, x, y);
        }
    }
    return result;
}

bool SceneFacade::trace_ray(I_KdTree *kd, Ray ray, Color *color, int depth)
{
    debug("trace_ray depth %d\n", depth);
    if (depth > MAX_DEPTH)  // stop recursing
        return false;

    double t_min = INFINITY;
    Vector3D n_min;
    SceneObject *hitObject = NULL;
    ray.direction.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<SceneObject*>::iterator it = objects_->begin(); it != objects_->end(); ++it)
    {
        SceneObject *obj = (*it);

        Vector3D n;
        double t = obj->intersect(ray.origin, ray.direction, &n);      // whats the n?

        if (0 <= t && t < t_min)
        {
            t_min = t;
            if (n.dot(ray.direction) >= 0)
                n = -n;
            n_min = n;
            hitObject = obj;
        }
    }

    if (hitObject == NULL)              // check for no intersection
    {
        return false;
    }

    Point3D p_int = ray.origin + t_min * ray.direction;      // calculate intersection point

    p_int = p_int + 0.001 * n_min;      // pull back point a bit, avoid self intersection

    // found closest intersection
    Point3D p = ray.origin + t_min * ray.direction;

    Vector3D n = n_min;

    // add radiance estimate
    SurfacePoint end_pt = SurfacePoint(p_int, n_min, hitObject->material);
    Color brdf = radiance_estimate(kd, end_pt);

    // calculate reflect vector
    Vector3D r = ray.direction + (2 * n_min.dot(-ray.direction)) * n_min;

    Ray new_ray = Ray(p_int, r);
    Color reflect_rgb;

    if (!trace_ray(kd, new_ray, &reflect_rgb, depth + 1))   //reflect color
        reflect_rgb = Color(0,0,0);

    // add reflection color
    col = col + brdf + reflect_rgb * end_pt.material->Kr;
    return true;
}

bool SceneFacade::trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth)
{
    debug("trace_ray_lights depth %d\n", depth);

    double t_min = INFINITY;
    Vector3D n_min;
    Light *hitLight = NULL;
    v.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<Light*>::iterator it = lights_->begin(); it != lights_->end(); ++it)
    {
        Light *obj = (*it);

        Vector3D n;
        double t = obj->intersect(o, v, &n);      // whats the n?

        if (0 <= t && t < t_min)
        {
            t_min = t;
            if (n.dot(v) >= 0)
                n = -n;
            n_min = n;
            hitLight = obj;
        }
    }

    if (hitLight == NULL)              // check for no intersection
    {
        return false;
    }

    // add emission color
    col = hitLight->clr;
    return true;
}


void SceneFacade::Transform(Matrix4x4 m)
{
    for(std::vector<SceneObject*>::iterator it = objects_->begin(); it != objects_->end(); ++it)
    {
        SceneObject *obj = (*it);
        obj->Transform(m);
    }
    for(std::vector<Light*>::iterator it = lights_->begin(); it != lights_->end(); ++it)
    {
        Light *obj = (*it);
        obj->Transform(m);
    }
}

Camera *SceneFacade::cam()
{
	return cam_;
}

ImagePlane *SceneFacade::imgPlane()
{
	return imgPlane_;
}

std::vector<SceneObject*> *SceneFacade::objects()
{
	return objects_;
}

std::vector<Light*> *SceneFacade::lights()
{
	return lights_;
}

int SceneFacade::imageWidth()
{
	return scene_->cam()->imgWidth;
}

int SceneFacade::imageHeight()
{
	return scene_->cam()->imgHeight;
}