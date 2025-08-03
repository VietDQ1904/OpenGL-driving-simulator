#include <iostream>
#include <vector>

#ifndef PERLIN_NOISE
#define PERLIN_NOISE


class PerlinNoise{
   public:
      PerlinNoise(unsigned int seed); // random seed constructor
      PerlinNoise(); // default constructor
      double getNoise(double x, double y, double z);
   
   private:
      std::vector<int> permTable;
      double fade(double t);
      double lerp(double t, double a, double b);
      double grad(int hash, double x, double y, double z);

};

#endif