#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef NOISE_MULTIPLIERS
#define NOISE_MULTIPLIERS

inline float pointToSegmentDistance(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b)
{
   glm::vec3 ab = b - a;
   glm::vec3 ap = p - a;
   float t = glm::dot(ap, ab) / glm::dot(ab, ab);
   t = glm::clamp(t, 0.0f, 1.0f);
   glm::vec3 projection = a + t * ab;
   return glm::length(p - projection);
}

inline float getNoiseMultiplierByDistance(float referenceDistance, float distance)
{
   float value = glm::clamp(distance / referenceDistance - 1.0f, 0.0f, 1.0f);
   value = std::pow(value, 3) * 2.0f;
   return value;
}

#endif