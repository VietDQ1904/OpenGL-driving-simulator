#include <array>
#include <cmath>
#include <math.h>
#include <limits>
#include <iostream>

#ifndef WORLEY_NOISE
#define WORLEY_NOISE

class WorleyNoise
{
public:
   WorleyNoise();
   WorleyNoise(unsigned int seed);
   double getNoise(double x, double y, double z);

private:
   unsigned int seed;
   unsigned int hash(unsigned int x, unsigned int y, unsigned int z);
   std::array<float, 3> getRandomPointInCell(unsigned int cellX, unsigned int cellY, unsigned int cellZ);
};

#endif