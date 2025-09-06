#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef UTILITY_FUNCTIONS
#define UTILITY_FUNCTIONS

inline glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
{
   glm::vec3 c = a + (b - a) * t;
   return c;
}

inline float getRandomFloatNumber(std::mt19937 &engine, float start, float end, unsigned int seed)
{
   std::uniform_real_distribution<float> distribution(start, end);
   float rand = distribution(engine);
   return rand;
}

inline int getRandomIntegerNumber(std::mt19937 &engine, int start, int end, unsigned int seed)
{
   std::uniform_int_distribution<int> distribution(start, end);
   int rand = distribution(engine);
   return rand;
}

#endif