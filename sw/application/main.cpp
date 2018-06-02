
#include "algebra.h"
#include "kdtree/kdtree.h"
#include "misc.h"
#include "scene.h"
#include <assert.h>
#include <cornellBoxScene.h>
#include <I_KdTree.h>
#include <I_Scene.h>
#include <lodepng.h>
#include <iostream>
#include <photonKdTree.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#define DEF_WIDTH   10
#define DEF_HEIGHT  10

double clamp(double min, double max, double in)
{
    if (in > max)
        return max;
    if (in < min)
        return min;
    return in;
}

void save_color_image (const char *filename, Color *image, int width, int height)
{
    std::vector<unsigned char> img;
    img.resize(width * height * 4);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int idx = x + y * width;
            Color &c = image[idx];
            // clamp rgb values [0,1]
            img[4 * idx + 0] = clamp(0, 1, c.R()) * 255;
            img[4 * idx + 1] = clamp(0, 1, c.G()) * 255;
            img[4 * idx + 2] = clamp(0, 1, c.B()) * 255;
            img[4 * idx + 3] = 255;
        }
    }

    // save to file
    lodepng::encode(filename, img, width, height);
}

I_KdTree* make_kdtree(std::vector<photon*> *photon_map)
{
    std::vector<photon> photon_map2;

    for (std::vector<photon*>::iterator it = photon_map->begin(); it != photon_map->end(); ++it)
    {
        photon *obj = *it;
        photon_map2.push_back(*obj);
    }

    I_KdTree *kd = new PhotonKdTree(
        new KdTree<photon, L2Norm_2, GetDim,3,float>(
            photon_map2));
    return kd;
}

void render_photons(I_Scene &scene, vector<photon*> *photon_map, const char* outputStr)
{
    int width = scene.imageWidth();
    int height = scene.imageHeight();

    Color *resultImg = scene.Render(photon_map);

    save_color_image(outputStr, resultImg, width, height);
    delete [] resultImg;
}


void normal_render(I_Scene &scene, const char* outputStr)
{
    int width = scene.imageWidth();
    int height = scene.imageHeight();

    // create new image
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    Color *resultImg = scene.Render();

    save_color_image(outputStr, resultImg, width, height);
    delete [] resultImg;
}

void final_render(I_Scene &scene, vector<photon*> *photons, const char* outputStr)
{
    int width = scene.imageWidth();
    int height = scene.imageHeight();
    I_KdTree *tree = make_kdtree(photons);

    Color *resultImg = scene.Render(tree);

    save_color_image(outputStr, resultImg, width, height);
    delete tree;
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


    normal_render(*scene, "standard.png");
    render_photons(*scene, &photon_map, "photons.png");
    final_render(*scene, &photon_map, "final.png");
    delete_photon_map(photon_map);

    // stop timing
    clock_t end = clock();

    double t = (double)(end-start)/CLOCKS_PER_SEC;
    printf("\nRendering Time: %fs.\n",t);

    delete scene;
    // application successfully returned
    return 0;
}
