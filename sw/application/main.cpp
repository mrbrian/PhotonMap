
#include "algebra.h"
#include "kdtree/kdtree.h"
#include "misc.h"
#include "scene.h"
#include <assert.h>
#include <cornellBoxScene.h>
#include <I_Scene.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <vector>

#define DEF_WIDTH   750
#define DEF_HEIGHT  750

double clamp(double min, double max, double in)
{
    if (in > max)
        return max;
    if (in < min)
        return min;
    return in;
}

KdTree<photon,L2Norm_2,GetDim,3,float>* make_kdtree(vector<photon*> *photon_map)
{
    vector<photon> photon_map2;

    for (std::vector<photon*>::iterator it = photon_map->begin(); it != photon_map->end(); ++it)
    {
        photon *obj = *it;
        photon_map2.push_back(*obj);
    }

    KdTree<photon,L2Norm_2,GetDim,3,float> *kd = new KdTree<photon,L2Norm_2,GetDim,3,float>(photon_map2);
    return kd;
}

void render_photons(Scene &scene, vector<photon*> *photon_map, const char* outputStr)
{
    int width = scene.cam.imgWidth;
    int height = scene.cam.imgHeight;

    Color *resultImg = scene.Render(photon_map);

    misc::save_color_image(outputStr, resultImg, width, height);
    delete [] resultImg;
}


void normal_render(Scene &scene, const char* outputStr)
{
    int width = scene.cam.imgWidth;
    int height = scene.cam.imgHeight;

    // create new image
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    Color *resultImg = scene.Render();

    misc::save_color_image(outputStr, resultImg, width, height);
    delete [] resultImg;
}

void final_render(Scene &scene, vector<photon*> *photons, const char* outputStr)
{
    int width = scene.cam.imgWidth;
    int height = scene.cam.imgHeight;
    KdTree<photon,L2Norm_2,GetDim,3,float> *tree = make_kdtree(photons);

    Color *resultImg = scene.Render(tree);

    misc::save_color_image(outputStr, resultImg, width, height);
    delete [] resultImg;
}

void delete_photon_map(vector<photon*> &map)
{
    for (std::vector< photon* >::iterator it = map.begin() ; it != map.end(); ++it)
    {
        delete (*it);
    }
    map.clear();
}

int main(int argc, char *argv[])
{
    I_Scene *scene;

    // image width and height
    int width  = DEF_WIDTH;
    int height = DEF_HEIGHT;

    if (argc >= 3)                  // prompt user on command line for dimensions
    {
        width  = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    scene = new CornellBoxScene(width, height);

    // start timing...
    clock_t start = clock();

    vector<photon*> photon_map;
    scene->emit_photons(10, &photon_map);

    delete_photon_map(photon_map);

    // normal_render(scene, "standard.png");
    // render_photons(scene, &photon_map, "photons.png");
    // final_render(scene, &photon_map, "final.png");

    // stop timing
    clock_t end = clock();

    double t = (double)(end-start)/CLOCKS_PER_SEC;
    printf("\nRendering Time: %fs.\n",t);

    delete scene;
    // application successfully returned
    return 0;
}
