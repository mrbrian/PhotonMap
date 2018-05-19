#ifndef MISC_H
#define MISC_H

#include <random>
#include "lodepng/lodepng.h"
#include "algebra.h"

class misc
{
public:
    static double RAND_1();
    static double RAND_2();
    static void debug(const char *s);
    static double clamp(double min, double max, double in);
    static double degToRad(float deg);
    static void save_color_image (const char *filename, Color *image, int width, int height);
};

#endif // MISC_H
