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
