#include <camera.h>
#include <I_Scene.h>
#include <imagePlane.h>
#include <Light.h>
#include <PhotonMappingSceneRenderer.h>
#include <Ray.h>
#include <PhotonMap.h>
#include <sceneObject.h>
#include <photon.h>
#include <I_KdTree.h>
#include <I_PhotonMap.h>

PhotonMappingSceneRenderer::PhotonMappingSceneRenderer(
	I_Scene &scene, 
	I_KdTree &kd, 
	int num_samples,
	int width, 
	int height)
	: scene_(scene)
	, kd_(kd)
	, imageWidth_(width)
	, imageHeight_(height)
{
    MAX_DEPTH = 5;
    BG_COLOR = Color(1,0,0);
    NORM_EPSILON = 0.001;
    samples_per_pixel_ = num_samples;
}

PhotonMappingSceneRenderer::~PhotonMappingSceneRenderer()
{

}

Color PhotonMappingSceneRenderer::radiance_estimate(I_KdTree *kd, SurfacePoint end_pt)
{
    // how much light is at this point?
    // locate k nearest photons
    // how much light from each

    const photon p(end_pt.position, Vector3D(0,1,0), Color(0,0,0), 0);
    std::vector<photon> nearest = kd->getKNearest(p, samples_per_pixel_);

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

Color PhotonMappingSceneRenderer::Render(I_KdTree *kd, int x, int y)
{
    // iterate over the pixels & set colour values
    // determine ray vector
    Point3D p = imagePlane()->ImageToWorldSpace(x, y, cam()->imgWidth, cam()->imgHeight);
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

Color *PhotonMappingSceneRenderer::Render()
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
            result[x + y * cam()->imgWidth] = Render(&kd_, x, y);
        }
    }
    return result;
}

bool PhotonMappingSceneRenderer::trace_ray(I_KdTree *kd, Ray ray, Color *color, int depth)
{
    // debug("trace_ray depth %d\n", depth);
    if (depth > MAX_DEPTH)  // stop recursing
        return false;

    double t_min = INFINITY;
    Vector3D n_min;
    SceneObject *hitObject = NULL;
    ray.direction.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<SceneObject*>::iterator it = objects()->begin(); it != objects()->end(); ++it)
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

bool PhotonMappingSceneRenderer::trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth)
{
    // debug("trace_ray_lights depth %d\n", depth);
    double t_min = INFINITY;
    Vector3D n_min;
    Light *hitLight = NULL;
    v.normalize();

    Color &col = *color;
    if (depth == 1)         // start ... with no colour
        col = Color(0,0,0);

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
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

int PhotonMappingSceneRenderer::imageWidth()
{
	return imageWidth_;
}

int PhotonMappingSceneRenderer::imageHeight()
{
	return imageHeight_;
}

Camera *PhotonMappingSceneRenderer::cam()
{
	return scene_.cam();
}

ImagePlane *PhotonMappingSceneRenderer::imagePlane()
{
	return scene_.imgPlane();
}

std::vector<SceneObject*> *PhotonMappingSceneRenderer::objects()
{
	return scene_.objects();
}

std::vector<Light*> *PhotonMappingSceneRenderer::lights()
{
	return scene_.lights();
}

// phong brdf
// x = surfact point
// pd = incident direction
// v = reflected direction

Color PhotonMappingSceneRenderer::BRDF(SurfacePoint &x, Vector3D& view, Vector3D& pd)
{
    Vector3D ref_dir = pd + (2 * x.normal.dot(-pd)) * x.normal;

    double pd_dot_norm = std::max(0.0, -pd.dot(x.normal));
    double ref_dot_view = std::max(0.0, -ref_dir.dot(view));

    Color diff = pd_dot_norm * x.material->Kd;
    Color spec = ref_dot_view * x.material->Ks;

    return (diff + spec);
}
