#include "worleyNoise.hpp"

WorleyNoise::WorleyNoise() : seed(255)
{
}

WorleyNoise::WorleyNoise(unsigned int seed) : seed(seed)
{
}

unsigned int WorleyNoise::hash(unsigned int x, unsigned int y, unsigned int z)
{
   unsigned int h = seed;
   h ^= x * 0x27d4eb2d;
   h ^= y * 0x165667b1;
   h ^= z * 0x9e3779b9;
   h ^= (h >> 15);
   h *= 0x85ebca6b;
   h ^= (h >> 13);
   return h;
}

std::array<float, 3> WorleyNoise::getRandomPointInCell(unsigned int cellX, unsigned int cellY, unsigned int cellZ)
{
   unsigned int h = hash(cellX, cellY, cellZ);
   float randomX = ((h & 0xFFFF) / 65535.0f);
   float randomY = (((h >> 8) & 0xFFFF) / 65535.0f);
   float randomZ = (((h >> 16) & 0xFFFF) / 65535.0f);

   return {randomX, randomY, randomZ};
}

double WorleyNoise::getNoise(double x, double y, double z)
{
   int ix = static_cast<int>(std::floor(x));
   int iy = static_cast<int>(std::floor(y));
   int iz = static_cast<int>(std::floor(z));

   double minDistance = std::numeric_limits<double>::infinity();

   for (int dx = -1; dx <= 1; ++dx)
   {
      for (int dy = -1; dy <= 1; ++dy)
      {
         for (int dz = -1; dz <= 1; ++dz)
         {
            int cx = ix + dx;
            int cy = iy + dy;
            int cz = iz + dz;

            auto point = getRandomPointInCell(cx, cy, cz);
            double coordX = cx + point[0];
            double coordY = cy + point[1];
            double coordZ = cz + point[2];

            double distX = coordX - x;
            double distY = coordY - y;
            double distZ = coordZ - z;

            double distance = std::sqrt(distX * distX + distY * distY + distZ * distZ);

            if (distance < minDistance)
            {
               minDistance = distance;
            }
         }
      }
   }

   return std::max(std::min(minDistance, 1.0), 0.0);
}