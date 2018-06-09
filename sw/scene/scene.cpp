#include <I_KdTree.h>
#include <imagePlane.h>
#include <Light.h>
#include <LightObject.h>
#include <Quad.h>
#include <scene.h>

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


Scene::Scene(
	Camera *cam,
	ImagePlane *imgPlane,
	std::vector<Light*> *lights,
	std::vector<I_SceneObject*> *objects)
	: cam_(cam)
	, imgPlane_ (imgPlane)
	, lights_(lights)
	, objects_(objects)
{
}

Scene::Scene(int num_samples)
    : samples_per_pixel_(num_samples)
{
}

Scene::~Scene()
{
    for (std::vector<I_SceneObject* >::iterator it = objects_->begin() ; it != objects_->end(); ++it)
    {
        delete (*it);
    }
    objects_->clear();
    delete imgPlane_;
}

void Scene::Transform(Matrix4x4 m)
{
    for(std::vector<I_SceneObject*>::iterator it = objects_->begin(); it != objects_->end(); ++it)
    {
        I_SceneObject *obj = (*it);
        obj->Transform(m);
    }
    for(std::vector<Light*>::iterator it = lights_->begin(); it != lights_->end(); ++it)
    {
        Light *obj = (*it);
        obj->Transform(m);
    }
}
/*
// cornell scene
Scene *Scene::planeScene(int width, int height, int num_samples)
{
    Scene *s = new Scene(num_samples);
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
*/
Camera *Scene::cam() 
{
	return cam_;
}

ImagePlane *Scene::imgPlane()
{
	return imgPlane_;
}

std::vector<I_SceneObject*> *Scene::objects()
{
	return objects_;
}

std::vector<Light*> *Scene::lights()
{
	return lights_;
}