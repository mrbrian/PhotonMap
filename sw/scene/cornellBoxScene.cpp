#include <cornellBoxScene.h>
#include <imagePlane.h>
#include <LightObject.h>
#include <material.h>
#include <quad.h>
#include <scene.h>
#include <shapes.h>

CornellBoxScene::CornellBoxScene(int width, int height)
{
    Camera *cam = new Camera();

    //cam.lookAt = Point3D(0, 0, -1);
    cam->imgWidth = width;
    cam->imgHeight = height;
    cam->position = Point3D(0, 0, 0);
    cam->fov = 53.1301024 / 180 * M_PI;
    cam->near = 1;
    cam->far = 10;
    cam->aspect = 1;//(float)width / height;
    cam->m_view = Matrix4x4::translation(Vector3D(0, 0,0));
    cam->m_view = Matrix4x4::rotation(M_PI, 'y');
    // float img_plane_w = 0.5f;
    //cam.m_view = Matrix4x4::rotation(0.025f, 'x') * cam.m_view;

    mat_ceil  = new Material(Color(0, 0, 0), Color(1, 1, 1), Color(0, 0, 0), 1, Color(0, 0, 0));
    mat_grn   = new Material(Color(0, 0, 0), Color(0, 0.5f, 0), Color(0, 0, 0), 10, Color(0, 0, 0));
    mat_red   = new Material(Color(0, 0, 0), Color(0.5f, 0, 0), Color(0, 0, 0), 10, Color(0, 0, 0));
    mat_light = new Material(Color(0, 0, 0), Color(0.5f, 0, 0), Color(1, 1, 1), 10, Color(0, 0, 0));
    mat_shiny = new Material(Color(0, 0, 0), Color(0,0,0), Color(1, 1, 1), 10, Color(1,1,1));
    mat_floor = new Material(Color(0, 0, 0), Color(0.6f, 0.6f, 0.6f), Color(0, 0, 0), 10, Color(0, 0, 0));

	std::vector<Light*> *lights = new std::vector<Light*>();
	std::vector<SceneObject*> *objects = new std::vector<SceneObject*>();
    // Ceiling
    Quad *q_1 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(2.75, 2.75, -5),
        Point3D(-2.75, 2.75, -5),
        Point3D(-2.75, 2.75, -10.5),
        mat_ceil);

    objects->push_back(q_1);

    // Ceiling light
    Quad *light_q = new Quad(
        Point3D(0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -8.274),
        Point3D(-0.653, 2.74, -7.224),
        Point3D(0.653, 2.74, -7.224),
        mat_light);

    l_obj = new LightObject(Point3D(0, 2.65, -8), Color(1, 1, 1), 20, light_q);
    lights->push_back(l_obj);

    // Green wall on left
    Quad *q_2 = new Quad(
        Point3D(-2.75, 2.75, -10.5),
        Point3D(-2.75, 2.75, -5),
        Point3D(-2.75, -2.75, -5),
        Point3D(-2.75, -2.75, -10.5),
        mat_grn);
    objects->push_back(q_2);

    //   // Red wall on right
    Quad *q_3 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(2.75, -2.75, -10.5),
        Point3D(2.75, -2.75, -5),
        Point3D(2.75, 2.75, -5),
        mat_red);
    objects->push_back(q_3);

    //   // Floor
    Quad *q_4 = new Quad(
        Point3D(2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -10.5),
        Point3D(-2.75, -2.75, -5),
        Point3D(2.75, -2.75, 5),
        mat_floor);
    objects->push_back(q_4);

    // Back wall
    Quad *q_5 = new Quad(
        Point3D(2.75, 2.75, -10.5),
        Point3D(-2.75, 2.75, -10.5),
        Point3D(-2.75, -2.75, -10.5),
        Point3D(2.75, -2.75, -10.5),
        mat_floor);
    objects->push_back(q_5);

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
    objects->push_back(big_cube);

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
    objects->push_back(sml_cube);

    scene = new Scene(
        cam,
        cam->calc_img_plane(),
        lights,
        objects);
}

CornellBoxScene::~CornellBoxScene()
{
    delete scene;
    delete mat_ceil;
    delete mat_grn;
    delete mat_red;
    delete mat_light;
    delete mat_shiny;
    delete mat_floor;
    delete l_obj;
}

Camera *CornellBoxScene::cam()
{
	return scene->cam();
}

ImagePlane *CornellBoxScene::imgPlane()
{
	return scene->imgPlane();
}

std::vector<SceneObject*> *CornellBoxScene::objects()
{
	return scene->objects();
}

std::vector<Light*> *CornellBoxScene::lights()
{
	return scene->lights();
}

void CornellBoxScene::Transform(Matrix4x4 m)
{
	scene->Transform(m);
}
