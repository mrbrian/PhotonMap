#include <RasterSceneRenderer.h>
#include <algebra.h>
#include <assert.h>
#include <cornellBoxScene.h>
#include <I_KdTree.h>
#include <I_Scene.h>
#include <I_SceneRenderer.h>
#include <iostream>
#include <kdtree/kdtree.h>
#include <lodepng.h>
#include <misc.h>
#include <photonKdTree.h>
#include <PhotonMappingSceneRenderer.h>
#include <PhotonSceneRenderer.h>
#include <scene.h>
#include <stdio.h>
#include <time.h>
#include <vector>

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

int main(int argc, char *argv[])
{
	I_Scene *scene;

    // image width and height
    int width  = atoi(argv[1]);
    int height = atoi(argv[2]);
    int samples = atoi(argv[3]);
    int num_photons = atoi(argv[4]);

    scene = new CornellBoxScene(width, height);

    // start timing...
    clock_t start = clock();

    RasterSceneRenderer normal_renderer(*scene);
    save_color_image("standard.png", normal_renderer.Render(), width, height);
    PhotonSceneRenderer photon_renderer(*scene, num_photons);
    save_color_image("photons.png", photon_renderer.Render(), width, height);

	PhotonKdTree kd(photon_renderer.photonMap());

    PhotonMappingSceneRenderer photon_mapping_renderer(*scene, kd, samples, width, height);
    save_color_image("final.png", photon_mapping_renderer.Render(), width, height);
    // delete_photon_map(photon_map);

    // stop timing
    clock_t end = clock();

    double t = (double)(end-start)/CLOCKS_PER_SEC;
    printf("\nRendering Time: %fs.\n",t);

    delete scene;
    // application successfully returned
    return 0;
}
