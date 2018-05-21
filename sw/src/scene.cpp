#include <Light.h>
#include <LightObject.h>
#include <scene.h>

//#define DEBUG 1
//#define debugging_enabled 1

const double m_samples_per_pixel = 10;

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
void BuildOrthonormalSystem(const Vector3D& v1, Vector3D& v2, Vector3D& v3)
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

// ----------------------------------------------------------------------------
// HemisphereSampling ()
//
// Generating outgoing ray directions by uniform sampling on a hemisphere
//
// Input: normal vector at the intersection point
// Output: outgoing ray direction from uniform sampling on a hemisphere
//
// ----------------------------------------------------------------------------

Vector3D HemisphereSampling(Vector3D m_normal)
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
    BuildOrthonormalSystem (m_normal, v2, v3);

    // Construct the normalized rotated vector
    double vecx = Vector3D(v2[0], v3[0], m_normal[0]).dot(sampled_ray_direction);
    double vecy = Vector3D(v2[1], v3[1], m_normal[1]).dot(sampled_ray_direction);
    double vecz = Vector3D(v2[2], v3[2], m_normal[2]).dot(sampled_ray_direction);
    Vector3D m_rotated_ray_direction = Vector3D(vecx,vecy,vecz);

    m_rotated_ray_direction.normalize();
    return m_rotated_ray_direction;
}

Color *Scene::Render()
{
    Color * result = new Color[cam.imgWidth * cam.imgHeight];
    // iterate over the pixels & set colour values
    for (int x = 0; x < cam.imgWidth; x++)
    {
        for (int y = 0; y < cam.imgHeight; y++)
        {
            // determine ray vector
            Point3D p = imgPlane->ImageToWorldSpace(x, y, cam.imgWidth, cam.imgHeight);
            Vector3D v = p - cam.position;
            v.normalize();

            Color &c = result[x + y * cam.imgWidth];

            if (!trace_ray_lights(cam.position, v, &c, 1))   // if ray hit nothing
                if (!trace_ray(cam.position, v, &c, 1))   // if ray hit nothing
                    c = BG_COLOR;                               // use background color
        }
    }
    return result;
}

Point2D Scene::calc_image_coords(Point3D pt)
{
    Matrix4x4 m_view = cam.get_view_matrix();
    Matrix4x4 *m_projection = cam.calc_proj_matrix();

    float width = cam.imgWidth;
    float height = cam.imgHeight;

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

Color *Scene::Render(vector<photon*> *photon_map)
{
    Color * result = new Color[cam.imgWidth * cam.imgHeight];
    // iterate over the pixels & set colour values

    for (int x = 0; x < cam.imgWidth; x++)
    {
        for (int y = 0; y < cam.imgHeight; y++)
        {
            result[x + y * cam.imgWidth] = Color(0, 0, 0);  // initialize to all black
        }
    }

    for(std::vector<photon*>::iterator it = photon_map->begin(); it != photon_map->end(); ++it)
    {
        photon *p = (*it);

        // calc image space coordinates
        Point2D img_coords = calc_image_coords(p->get_position());
        int x = (int)img_coords[0];
        int y = (int)img_coords[1];
        int idx = x + y * cam.imgWidth;
        if (idx < 0 || idx >= cam.imgHeight * cam.imgWidth)
            continue;
        Color &pixel = result[x + y * cam.imgWidth];
        pixel = pixel + *(p->get_color());
    }

    return result;
}

// check if objects lay between a point and light position
bool intersection_test(Scene *scene, Point3D o, Point3D light_pos)
{
    Vector3D v = light_pos - o;
    double dist = v.length();
    v.normalize();

    float tmin = INFINITY;

    for(std::vector<SceneObject*>::iterator it = scene->objects.begin(); it != scene->objects.end(); ++it)
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

bool Scene::trace_ray(Point3D o, Vector3D v, Color *color, int depth)
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

    for(std::vector<SceneObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
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

    for(std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
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


SceneObject *find_closest_intersection(Scene *scene, Point3D o, Vector3D v, double *t_min_ptr, Vector3D *n_min_ptr)
{
    SceneObject *hitObject = NULL;
    double &t_min = *t_min_ptr;
    Vector3D &n_min = *n_min_ptr;

    for (std::vector<SceneObject*>::iterator it = scene->objects.begin(); it != scene->objects.end(); ++it)
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

bool Scene::trace_primary_ray(Point3D in_pos, Vector3D in_dir, Color *in_clr, Point3D *_out_pos, Vector3D *_out_norm, Vector3D *_out_reflect, Vector3D *_out_refract, Color *_out_clr, Material *_out_mat)
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
void Scene::trace_photon(photon *in_pho, int depth, vector<photon*> *out_list)
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

void Scene::emit_photons(int num_photons, vector<photon*> *photon_map)
{
    vector<photon*> photons;

    initialize_photons(num_photons, &photons);    // spawn initial photons for all lights

    for(std::vector<photon*>::iterator it = photons.begin(); it != photons.end(); ++it)
    {
        photon *obj = (*it);
        trace_photon(obj, 0, photon_map);
    }
}

void Scene::bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_normal, Vector3D *i_reflect,
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
}

Scene::RayType Scene::russian_roulette(Material *mat)   // [0, d] diffuse reflection ray, [d,s+d] specular ray, [s+d, 1] absorption
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

void Scene::initialize_photons(int num_photons, vector<photon*> *out_photons)
{
    double total_watts = 0;
    double energy_per_photon = 0;

    for(std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        Light *obj = (*it);
        total_watts += obj->watts;
    }

    energy_per_photon = total_watts / num_photons;

    for(std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
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

Color Scene::BRDF(SurfacePoint &x, Vector3D& view, Vector3D& pd)
{
    Vector3D ref_dir = pd + (2 * x.normal.dot(-pd)) * x.normal;

    double pd_dot_norm = std::max(0.0, -pd.dot(x.normal));
    double ref_dot_view = std::max(0.0, -ref_dir.dot(view));

    Color diff = pd_dot_norm * x.material->Kd;
    Color spec = ref_dot_view * x.material->Ks;

    return (diff + spec);
}

Color Scene::radiance_estimate(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, SurfacePoint end_pt)
{
    // how much light is at this point?
    // locate k nearest photons
    // how much light from each

    const photon p = photon(end_pt.position, Vector3D(0,1,0), Color(0,0,0), 0);
    vector<photon> nearest = kd->getKNearest(p, m_samples_per_pixel);

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

        Vector3D eye_dir = pho.get_direction() - cam.position; // eye direction

        Color brdf = BRDF(end_pt, eye_dir, pd);
        flux = flux + brdf * pw * pho.color;
    }

//    debug("radius2 %f\n", r_2);
    flux = flux / (M_PI * r_2);
    return flux;
}

Color Scene::Render(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, int x, int y)
{
    // iterate over the pixels & set colour values
    // determine ray vector
    Point3D p = imgPlane->ImageToWorldSpace(x, y, cam.imgWidth, cam.imgHeight);
    Vector3D v = p - cam.position;
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

Color *Scene::Render(KdTree<photon,L2Norm_2,GetDim,3,float> *kd)
{
    Color * result = new Color[cam.imgWidth * cam.imgHeight];
    // iterate over the pixels & set colour values
    //#pragma omp parallel for schedule(dynamic)

    for (int x = 0; x < cam.imgWidth; x++)
    {
        float perc = (float) x / cam.imgWidth * 100;
        fprintf(stdout,"\rRendering: %1.0f Samples per Pixel %8.2f%%", m_samples_per_pixel, (double)perc);

        for (int y = 0; y < cam.imgHeight; y++)
        {

            result[x + y * cam.imgWidth] = Render(kd, x, y);
        }
    }
    return result;
}

bool Scene::trace_ray(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, Ray ray, Color *color, int depth)
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

    for(std::vector<SceneObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
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

bool Scene::trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth)
{
    debug("trace_ray_lights depth %d\n", depth);

    double t_min = INFINITY;
    Vector3D n_min;
    Light *hitLight = NULL;
    v.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
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


// cornell scene
Scene *Scene::cornellBoxScene(int width, int height)
{
    Scene *s = new Scene();
    Scene &scene = *s;
    Camera cam = Camera();

    cam.imgWidth = width;
    cam.imgHeight = height;
    //cam.lookAt = Point3D(0, 0, -1);
    cam.position = Point3D(0, 0, 0);
    cam.fov = 53.1301024 / 180 * M_PI;
    cam.near = 1;
    cam.far = 10;
    cam.aspect = 1;//(float)width / height;

    scene.cam = cam;
    float img_plane_w = 0.5f;
    cam.m_view = Matrix4x4::translation(Vector3D(0, 0,0));
    cam.m_view = Matrix4x4::rotation(M_PI, 'y');
    //cam.m_view = Matrix4x4::rotation(0.025f, 'x') * cam.m_view;

    scene.imgPlane = cam.calc_img_plane();
    for (int y = 0; y < 4; y++)
    {
        debug("%f %f %f\n", scene.imgPlane->points[y][0], scene.imgPlane->points[y][1], scene.imgPlane->points[y][2], scene.imgPlane->points[y][3]);
    }

    Material *mat_ceil  = new Material(Color(0, 0, 0), Color(1, 1, 1), Color(0, 0, 0), 1, Color(0, 0, 0));
    Material *mat_grn   = new Material(Color(0, 0, 0), Color(0, 0.5f, 0), Color(0, 0, 0), 10, Color(0, 0, 0));
    Material *mat_red   = new Material(Color(0, 0, 0), Color(0.5f, 0, 0), Color(0, 0, 0), 10, Color(0, 0, 0));
    Material *mat_light = new Material(Color(0, 0, 0), Color(0.5f, 0, 0), Color(1, 1, 1), 10, Color(0, 0, 0));
    Material *mat_shiny = new Material(Color(0, 0, 0), Color(0,0,0), Color(1, 1, 1), 10, Color(1,1,1));
    Material *mat_floor = new Material(Color(0, 0, 0), Color(0.6f, 0.6f, 0.6f), Color(0, 0, 0), 10, Color(0, 0, 0));

    // Ceiling
    Quad *q_1 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(2.75, 2.75, -5),
        Point3D(-2.75, 2.75, -5),
        Point3D(-2.75, 2.75, -10.5),
        mat_ceil);

    scene.objects.push_back(q_1);

    // Ceiling light
    Quad *light_q = new Quad(
        Point3D(0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -7.224),
        Point3D(0.653, 2.74, -7.224),
        mat_light);

    LightObject *l_obj = new LightObject(Point3D(0, 2.65, -8), Color(1, 1, 1), 20, light_q);
    scene.lights.push_back(l_obj);

    // Green wall on left
    Quad *q_2 = new Quad(
        Point3D(-2.75, 2.75, -10.5),
        Point3D(-2.75, 2.75, -5),
        Point3D(-2.75, -2.75, -5),
        Point3D(-2.75, -2.75, -10.5),
        mat_grn);
    scene.objects.push_back(q_2);

    //   // Red wall on right
    Quad *q_3 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(2.75, -2.75, -10.5),
        Point3D(2.75, -2.75, -5),
        Point3D(2.75, 2.75, -5),
        mat_red);
    scene.objects.push_back(q_3);

    //   // Floor
    Quad *q_4 = new Quad(
        Point3D(2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -5),
        Point3D(2.75, -2.75, 5),
        mat_floor);
    scene.objects.push_back(q_4);

    // Back wall
    Quad *q_5 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(-2.75, 2.75, -10.5),
        Point3D(-2.75, -2.75, -10.5),
        Point3D(2.75, -2.75, -10.5),
        mat_floor);
    scene.objects.push_back(q_5);

    Cube *big_cube = new Cube(
        Point3D(0, 0, 0),
        0.5,
        mat_ceil
    );
    big_cube->Transform(
        Matrix4x4::translation(Vector3D(-0.929, -2.75 + 3.31 / 2, -8.482)) *
        Matrix4x4::rotation(misc::degToRad(-18.809), 'y') *
        Matrix4x4::scaling(Vector3D(1.659, 3.31, 1.659))
    );
    scene.objects.push_back(big_cube);

    Cube *sml_cube = new Cube(
        Point3D(0,0,0),
        0.5,
        mat_shiny
    );
    sml_cube->Transform(
        Matrix4x4::translation(Vector3D(0.933, -2.75 + 1.655 / 2, -6.648)) *
        Matrix4x4::rotation(misc::degToRad(16.303), 'y') *
        Matrix4x4::scaling(Vector3D(1.655, 1.655, 1.655))
    );
    scene.objects.push_back(sml_cube);
    return s;
}

void Scene::Transform(Matrix4x4 m)
{
    for(std::vector<SceneObject*>::iterator it = objects.begin(); it != objects.end(); ++it)
    {
        SceneObject *obj = (*it);
        obj->Transform(m);
    }
    for(std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
    {
        Light *obj = (*it);
        obj->Transform(m);
    }
}

// cornell scene
Scene *Scene::planeScene(int width, int height)
{
    Scene *s = new Scene();
    Scene &scene = *s;
    Camera cam = Camera();

    cam.imgWidth = width;
    cam.imgHeight = height;
    //cam.lookAt = Point3D(0, 0, -1);
    cam.position = Point3D(0, 0, 0);
    cam.fov = 53.1301024 / 180 * M_PI;
    cam.near = 1;
    cam.far = 10;
    cam.aspect = 1;//(float)width / height;

    scene.cam = cam;
    float img_plane_w = 0.5f;
    cam.m_view = Matrix4x4::translation(Vector3D(0,0,0));
    cam.m_view = cam.m_view.rotation(M_PI, 'y');

    scene.imgPlane = cam.calc_img_plane();

    Material *mat_light = new Material(Color(0, 0, 0), Color(0.5f, 0, 0), Color(1, 1, 1), 10, Color(0, 0, 0));
    Material *mat_floor = new Material(Color(0, 0, 0), Color(0.6f, 0.6f, 0.6f), Color(0, 0, 0), 10, Color(0, 0, 0));

    // Ceiling light
    Quad *light_q = new Quad(
        Point3D(0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -7.224),
        Point3D(0.653, 2.74, -7.224),
        mat_light);

    LightObject *l_obj = new LightObject(Point3D(0, 2.65, -8), Color(1, 1, 1), 10, light_q);
    scene.lights.push_back(l_obj);

    //   // Floor
    Quad *q_4 = new Quad(
        Point3D(2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -5),
        Point3D(2.75, -2.75, 5),
        mat_floor);
    scene.objects.push_back(q_4);

    return s;
}
