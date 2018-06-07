#include <camera.h>
#include <I_Scene.h>
#include <imagePlane.h>
#include <Light.h>
#include <RasterSceneRenderer.h>
#include <Ray.h>
#include <sceneObject.h>

RasterSceneRenderer::RasterSceneRenderer(I_Scene &scene)
	: scene_(scene)
{
    MAX_DEPTH = 5;
    BG_COLOR = Color(1,0,0);
    NORM_EPSILON = 0.001;
}

RasterSceneRenderer::~RasterSceneRenderer()
{
}

Color *RasterSceneRenderer::Render()
{
    Color * result = new Color[cam()->imgWidth * cam()->imgHeight];
    // iterate over the pixels & set colour values
    for (int x = 0; x < cam()->imgWidth; x++)
    {
        for (int y = 0; y < cam()->imgHeight; y++)
        {
            // determine ray vector
            Point3D p = imagePlane()->ImageToWorldSpace(x, y, cam()->imgWidth, cam()->imgHeight);
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

int RasterSceneRenderer::imageWidth()
{
	return imageWidth_;
}

int RasterSceneRenderer::imageHeight()
{
	return imageHeight_;
}

Camera *RasterSceneRenderer::cam()
{
    return scene_.cam();
}

ImagePlane *RasterSceneRenderer::imagePlane()
{
    return scene_.imgPlane();
}

bool RasterSceneRenderer::trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth)
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

bool RasterSceneRenderer::trace_ray(Point3D o, Vector3D v, Color *color, int depth)
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

    for(std::vector<SceneObject*>::iterator it = objects()->begin(); it != objects()->end(); ++it)
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

    for(std::vector<Light*>::iterator it = lights()->begin(); it != lights()->end(); ++it)
    {
        Light *light= (*it);
        col = col + hitObject->material->Ka;

        Point3D new_p = p + 0.1 * (light->position - p);
        if (intersection_test(&scene_, new_p, light->position))
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

std::vector<SceneObject*> *RasterSceneRenderer::objects()
{
    return scene_.objects();
}

std::vector<Light*> *RasterSceneRenderer::lights()
{
    return scene_.lights();
}

// check if objects lay between a point and light position
bool RasterSceneRenderer::intersection_test(I_Scene *scene, Point3D o, Point3D light_pos)
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