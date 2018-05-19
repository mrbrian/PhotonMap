#include "misc.h"

std::mt19937 MersenneTwisterPRNG;
std::uniform_real_distribution<double> m_URD;

#define DEBUG 0

void misc::debug(const char *s)
{
    if (DEBUG)
        printf(s);
}

double misc::RAND_1()
{
    return (2.0 * m_URD(MersenneTwisterPRNG) - 1.0);    // [-1,1]
}

double misc::RAND_2()
{
    return (m_URD(MersenneTwisterPRNG));    // [0,1]
}

double misc::clamp(double min, double max, double in)
{
    if (in > max)
        return max;
    if (in < min)
        return min;
    return in;
}

double misc::degToRad(float deg)
{
    return (M_PI / 180 * deg);
}

void misc::save_color_image (const char *filename, Color *image, int width, int height)
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
