#ifndef SCENE
#define SCENE

#include "shapes.h"
#include "algebra.h"
#include "photon.h"
#include <vector>
#include "polyroots.h"
#include "camera.h"
#include "kdtree/kdtree.h"
#include "misc.h"

class ImagePlane;
class Light;

using namespace std;

//l2 norm squared

struct L2Norm_2{
   L2Norm_2(){}
   float operator () (const photon &n1,const photon &n2)const{
      return n1.dist2(n2);
   }
};

struct GetDim{
   GetDim(){}
   float operator () (const photon &n1,unsigned dim)const{
    switch (dim)
    {
    case 0:
        return n1.x;
    case 1:
        return n1.y;
    case 2:
        return n1.z;
    default:
        printf("incorrect dim parameter\n");
        throw -1;
    }
   }
};

struct SurfacePoint
{
public:
    Point3D position;
    Vector3D normal;
    Material *material;

    SurfacePoint() {}

    SurfacePoint(Point3D p, Vector3D v, Material *m)
    {
        position = p;
        normal = v;
        material = m;
    }
};

struct Ray
{
public:
    Point3D origin;
    Vector3D direction;

    Ray(Point3D p, Vector3D v)
    {
        origin = p;
        direction = v;
    }
};

class Scene
{
public:
    enum RayType
    {
        Diffuse,
        Specular,
        Transmission
    };

    Scene();
    ~Scene();

    static Scene *planeScene(int width, int height);
    static Scene *cornellBoxScene(int width, int height);

    Color *Render();
    Color *Render(vector<photon*> *photon_map);
    Color *Render(KdTree<photon,L2Norm_2,GetDim,3,float> *kd);
    Color Render(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, int x, int y);

    bool trace_ray(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, Ray ray, Color *color, int depth);
    bool trace_ray(Point3D o, Vector3D v, Color *color, int depth);
    bool trace_ray_lights(Point3D o, Vector3D v, Color *color, int depth);
    bool trace_ray(Point3D o, Vector3D v, Color *color, Point3D *out_pos, Vector3D *out_norm, Color *out_clr, Material *out_mat, int depth);
    bool trace_primary_ray(Point3D in_pos, Vector3D in_dir, Color *in_clr, Point3D *_out_pos, Vector3D *_out_norm, Vector3D *_out_reflect, Vector3D *_out_refract, Color *_out_clr, Material *_out_mat);
    Point2D calc_image_coords(Point3D pt);

    // collide photon with the scene objects
    Color BRDF(SurfacePoint &x, Vector3D &view, Vector3D &pd);
    void initialize_photons(int num_photons, vector<photon*> *out_photons);
    RayType russian_roulette(Material *mat);
    void emit_photons(int num_photons, vector<photon*> *photon_map);
    void trace_photon(photon *in_pho, int depth, vector<photon*> *out_list);
    void bounce_photon(RayType ray_type, Point3D *i_pos, Vector3D *i_norm, Vector3D *i_reflect, Vector3D *i_refract, Color *i_clr, double energy, int depth, vector<photon*> *out_list);
    Color radiance_estimate(KdTree<photon,L2Norm_2,GetDim,3,float> *kd, SurfacePoint end_pt);

    void Transform(Matrix4x4 m);

    Camera cam;
    ImagePlane *imgPlane;
    std::vector<SceneObject*> objects;
    std::vector<Light*> lights;
};

void BuildOrthonormalSystem(const Vector3D& v1, Vector3D& v2, Vector3D& v3);
Vector3D HemisphereSampling(Vector3D m_normal);

#endif //SCENE
