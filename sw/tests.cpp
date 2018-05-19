#include "tests.h"
#include "scene.h"
#include <stdio.h>
#include <math.h>
#include "lodepng.h"

using namespace std;

double clamp(double min, double max, double in)
{
    if (in > max)
        return max;
    if (in < min)
        return min;
    return in;
}

void sphericalCoord_1()
{
    photon p;

    vector<Vector3D> list = {Vector3D(1,1,1), Vector3D(-1,-1,-1),
                             Vector3D(1, 0, 0), Vector3D(0, 1, 0), Vector3D(0, 0, 1), Vector3D(-1, 0, 0), Vector3D(0, -1, 0), Vector3D(0, 0, -1)};

    for(std::vector<Vector3D>::iterator it = list.begin(); it != list.end(); ++it)
    {
        Vector3D expected = (*it);
        expected.normalize();
        p.set_direction(expected);
        printf("%d, %d\n", p.phi, p.theta);
        Vector3D actual = p.get_direction();
        printf("%f, %f, %f\n", expected[0], expected[1], expected[2]);
        printf("%f, %f, %f\n", actual[0], actual[1], actual[2]);
        if ((expected - actual).length2() > 0.5)
            throw 1;
    }
}

#define DEF_VIEW_NEAR       1
#define DEF_VIEW_FAR        10
#define DEF_VIEW_FOV        90
#define DEF_VIEW_DIST       1

void proj_point()
{
    printf("proj_point\n");
    vector<photon*> p_list;
    {
        photon *p = new photon();
        p->set_position(Point3D(1,0,1));
        p->set_direction(Vector3D(0,0,1));
        p->set_color(Color(1,0,1));
        p_list.push_back(p);
    }
    photon *p2 = new photon();
    p2->set_position(Point3D(0,1,1));
    p2->set_direction(Vector3D(0,0,1));
    p2->set_color(Color(0,1,1));
    p_list.push_back(p2);

    photon *p3 = new photon();
    p3->set_position(Point3D(1,1,1));
    p3->set_direction(Vector3D(0,0,1));
    p3->set_color(Color(0,1,0));
    p_list.push_back(p3);

    {
        photon *p = new photon();
        p->set_position(Point3D(-1,-1,1));
        p->set_direction(Vector3D(1,0,0));
        p->set_color(Color(1,1,0));
        p_list.push_back(p);
    }

    Scene s;
    s.cam.position = Point3D(0,0,0);

    //float aspect = (float)width() / height();

    s.cam.fov = (float)DEF_VIEW_FOV / 180 * M_PI;
    s.cam.far = DEF_VIEW_FAR;
    s.cam.near = DEF_VIEW_NEAR;
    s.cam.aspect = 1;
    s.cam.imgHeight = 5;
    s.cam.imgWidth= 5;

    Matrix4x4 m;
    m.translation(Vector3D(0,0,0));
    s.cam.m_view = m;

    float img_plane_w = 0.5f;
    s.imgPlane = s.cam.calc_img_plane();
    for (int y = 0; y < 4; y++)
    {
        printf("%f %f %f\n", s.imgPlane->points[y][0], s.imgPlane->points[y][1], s.imgPlane->points[y][2], s.imgPlane->points[y][3]);
    }

    int width = s.cam.imgWidth;
    int height = s.cam.imgHeight;

    std::vector<unsigned char> img;
    img.resize(width * height * 4);

    Color *image = s.Render(&p_list);
    for (int y = 0; y < s.cam.imgHeight; y++)
    {
        for (int x = 0; x < s.cam.imgWidth; x++)
        {
            printf("%d,%d - %f %f %f\n", x,y,image[y*s.cam.imgWidth + x].R(), image[y*s.cam.imgWidth + x].G(), image[y*s.cam.imgWidth + x].B());
        }
    }

    for (int x = 0; x < s.cam.imgWidth; x++)
    {
        for (int y = 0; y < s.cam.imgHeight; y++)
        {
            int idx = x + y * s.cam.imgWidth;
            Color &c = image[idx];
            // clamp rgb values [0,1]
            img[4 * idx + 0] = clamp(0, 1, c.R()) * 255;
            img[4 * idx + 1] = clamp(0, 1, c.G()) * 255;
            img[4 * idx + 2] = clamp(0, 1, c.B()) * 255;
            img[4 * idx + 3] = 255;
        }
    }
    // save to file
    lodepng::encode("proj_point", img, s.cam.imgWidth, s.cam.imgHeight);

}

void proj_point2()
{
    vector<Point3D> photon_pos;


    printf("proj_point2\n");
    Matrix4x4 m;
    m = m.rotation(M_PI, 'y');
    vector<photon*> p_list;
    {
        photon *p = new photon();
        Point3D p_p = Point3D(1,0,1);
        p_p = m * p_p;
        p->set_position(p_p);
        p->set_direction(Vector3D(0,0,1));
        p->set_color(Color(1,0,1));
        p_list.push_back(p);
    }
    photon *p2 = new photon();
    Point3D p_p2 = Point3D(0,1,1);
    p_p2 = m * p_p2;
    p2->set_position(p_p2);
    p2->set_direction(Vector3D(0,0,1));
    p2->set_color(Color(0,1,1));
    p_list.push_back(p2);

    photon *p3 = new photon();
    Point3D p_p3 = Point3D(1,1,1);
    p_p3 = m * p_p3;
    p3->set_position(p_p3);
    p3->set_direction(Vector3D(0,0,1));
    p3->set_color(Color(0,1,0));
    p_list.push_back(p3);

    {
        photon *p = new photon();
        Point3D p_p = Point3D(-1,-1,1);
        p_p = m * p_p;
        p->set_position(p_p);
        p->set_direction(Vector3D(1,0,0));
        p->set_color(Color(1,1,0));
        p_list.push_back(p);
    }

    Scene s;
    s.cam.position = Point3D(0,0,0);

    //float aspect = (float)width() / height();

    s.cam.fov = (float)DEF_VIEW_FOV / 180 * M_PI;
    s.cam.far = DEF_VIEW_FAR;
    s.cam.near = DEF_VIEW_NEAR;
    s.cam.aspect = 1;
    s.cam.imgHeight = 5;
    s.cam.imgWidth = 5;

    s.cam.m_view = m;

    float img_plane_w = 0.5f;
    s.imgPlane = s.cam.calc_img_plane();
    for (int y = 0; y < 4; y++)
    {
        printf("%f %f %f\n", s.imgPlane->points[y][0], s.imgPlane->points[y][1], s.imgPlane->points[y][2], s.imgPlane->points[y][3]);
    }

    int width = s.cam.imgWidth;
    int height = s.cam.imgHeight;

    std::vector<unsigned char> img;
    img.resize(width * height * 4);

    Color *image = s.Render(&p_list);
    for (int y = 0; y < s.cam.imgHeight; y++)
    {
        for (int x = 0; x < s.cam.imgWidth; x++)
        {
            printf("%d,%d - %f %f %f\n", x,y,image[y*s.cam.imgWidth + x].R(), image[y*s.cam.imgWidth + x].G(), image[y*s.cam.imgWidth + x].B());
        }
    }

    for (int x = 0; x < s.cam.imgWidth; x++)
    {
        for (int y = 0; y < s.cam.imgHeight; y++)
        {
            int idx = x + y * s.cam.imgWidth;
            Color &c = image[idx];
            // clamp rgb values [0,1]
            img[4 * idx + 0] = clamp(0, 1, c.R()) * 255;
            img[4 * idx + 1] = clamp(0, 1, c.G()) * 255;
            img[4 * idx + 2] = clamp(0, 1, c.B()) * 255;
            img[4 * idx + 3] = 255;
        }
    }
    // save to file
    lodepng::encode("proj_point2", img, s.cam.imgWidth, s.cam.imgHeight);

}

void camera_plane()
{
    printf("camera_plane\n");
    Scene s;
    s.cam.position = Point3D(0,0,0);

    //float aspect = (float)width() / height();

    s.cam.fov = (float)DEF_VIEW_FOV / 180 * M_PI;
    s.cam.far = DEF_VIEW_FAR;
    s.cam.near = DEF_VIEW_NEAR;
    s.cam.aspect = 1;
    s.cam.imgHeight = 5;
    s.cam.imgWidth= 5;

    Matrix4x4 m;

    s.imgPlane = s.cam.calc_img_plane();

    for (int y = 0; y < 4; y++)
    {
        printf("%f %f %f\n", s.imgPlane->points[y][0], s.imgPlane->points[y][1], s.imgPlane->points[y][2], s.imgPlane->points[y][3]);
    }

    m = m.translation(Vector3D(0,0,0));
    m = m.rotation(M_PI, 'y');
    s.cam.m_view = m;

    Vector3D expected[4] = {
        Vector3D(0,0,0),
        Vector3D(0,0,0),
        Vector3D(0,0,0),
        Vector3D(0,0,0)
    };

    printf("camera_planeb\n");

    s.imgPlane = s.cam.calc_img_plane();

    for (int y = 0; y < 4; y++)
    {
        printf("%f %f %f\n", s.imgPlane->points[y][0], s.imgPlane->points[y][1], s.imgPlane->points[y][2]);
    }

    for (int x = 0; x < s.cam.imgWidth; x++)
    {
        for (int y = 0; y < s.cam.imgHeight; y++)
        {
            // determine ray vector
            Point3D p = s.imgPlane->ImageToWorldSpace(x, y, s.cam.imgWidth, s.cam.imgHeight);
            printf("%f %f %f\n", p[0], p[1], p[2]);
        }
    }
}

#define DEF_WIDTH   250
#define DEF_HEIGHT  250

void radiance()
{
    // setup scene
    // shoot photons
    // put in kd_tree
    // find nearest photon at some point

    Scene *scene = Scene::cornellBoxScene(DEF_WIDTH, DEF_HEIGHT);
    Scene &s = *scene;

    vector<photon*> photon_map;
    s.emit_photons(100, &photon_map);

    vector<photon> photon_map2;

    for (std::vector<photon*>::iterator it = photon_map.begin(); it != photon_map.end(); ++it)
    {
        photon *obj = *it;
        photon_map2.push_back(*obj);
    }

    KdTree<photon,L2Norm_2,GetDim,3,float>  kd(photon_map2);

    //double b[3] = {-1,2.75,-5.6};
    //kdres *result = kd_nearest_range(kd, b, 20);

    const photon p = photon(
                Point3D(0,0,0),
                Vector3D(0,1,0),
                Color(1,1,1),
                0
                );

    vector<photon> nearest = kd.getKNearest(p, 2);
    float x, y, z;
    vector<photon>::iterator it = nearest.begin();

    //if (data);
    /* pointer experiments
    Vector3D test1 = Vector3D(1,2,3);
    Vector3D *test2 = new Vector3D(6,6,6);

    test1 = *test2;
    test1[0] = 555;
    delete (test2);
    test2 = &test1;
    test1[0] = 115;*/

    //data->flag = 666;
    //kd_res_item

    SurfacePoint end_pt = SurfacePoint(Point3D(0,0,0),
                                       Vector3D(0,1,0),
                                       new Material(
                                           Color(1,1,1),
                                           Color(1,1,1),
                                           Color(1,1,1),
                                           1.0,
                                           Color(1,1,1)
                                           )
                                        );
    //Color flux = s.radiance_estimate(kd, end_pt);
    //Color flux2 = s.radiance_estimate(kd, end_pt);
    Color test0 = s.Render(&kd, 132, 116);
    printf("test0 - %f %f %f\n", test0.R(), test0.G(), test0.B());
    Color test1 = s.Render(&kd, 132, 117);
    printf("test1 - %f %f %f\n", test1.R(), test1.G(), test1.B());
	return;
    Color *img = s.Render(&kd);

    const char *filename = "radiance_test";
    misc::save_color_image(filename, img, s.cam.imgWidth, s.cam.imgHeight);
}

void render_square()
{
    Scene *scene = Scene::planeScene(DEF_WIDTH, DEF_HEIGHT);
    Scene &s = *scene;

    vector<photon*> photon_map;
    Point3D floor_top_left = Point3D(-2.75, -2.75, -10.5);
    int w = 1;
    int h = 1;

    for (int i = 0; i < 25; i++)
    {
        Point3D offs = Point3D(i % 5 * w, 0, i / 5 * h);
        photon *p = new photon(floor_top_left + offs, Vector3D(0,-1,0), Color(1,1,1), 0.25);
        photon_map.push_back(p);
    }

    vector<photon> photon_map2;

    for (std::vector<photon*>::iterator it = photon_map.begin(); it != photon_map.end(); ++it)
    {
        photon *obj = *it;
        photon_map2.push_back(*obj);
    }

    KdTree<photon,L2Norm_2,GetDim,3,float>  kd(photon_map2);

    const photon p = photon(
                Point3D(0,0,0),
                Vector3D(0,1,0),
                Color(1,1,1),
                0
                );

    vector<photon> nearest = kd.getKNearest(p, 2);
    float x, y, z;
    vector<photon>::iterator it = nearest.begin();

    SurfacePoint end_pt = SurfacePoint(Point3D(0,0,0),
                                       Vector3D(0,1,0),
                                       new Material(
                                           Color(1,1,1),
                                           Color(1,1,1),
                                           Color(1,1,1),
                                           1.0,
                                           Color(1,1,1)
                                           )
                                        );

    Color test0 = s.Render(&kd, 107, 191);
    printf("test0 - %f %f %f\n", test0.R(), test0.G(), test0.B());
    Color test1 = s.Render(&kd, 108, 191);
    printf("test1 - %f %f %f\n", test1.R(), test1.G(), test1.B());
    //return;
    Color *img = s.Render(&kd);
    const char *filename = "radiance_test";
    misc::save_color_image(filename, img, s.cam.imgWidth, s.cam.imgHeight);

    img = s.Render(&photon_map);

    filename = "photon_test";
    misc::save_color_image(filename, img, s.cam.imgWidth, s.cam.imgHeight);
}

tests::tests()
{
    /*sphericalCoord_1();
    proj_point();
    proj_point2();
    camera_plane();*/
    //radiance();
    render_square();
}

int main(int argc, char *argv[])
{
    tests t;

    return 0;
}
