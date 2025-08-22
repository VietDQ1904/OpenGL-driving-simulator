#include "roadSigns.hpp"

void RoadSigns::generateModels()
{

   glm::vec3 current;
   glm::vec3 next;
   bool firstCurve = false;
   float angle = 0.0f;
   float crossProduct;

   for (int i = 0; i < points.size() - 2; ++i)
   {
      current = points[i + 1] - points[i];
      next = points[i + 2] - points[i + 1];

      angle = glm::acos(glm::dot(current, next) / (glm::length(current) * glm::length(next)));

      if (glm::degrees(angle) >= 15.0f)
      {
         if (firstCurve)
         {
         }
      }
      else
      {
         firstCurve = false;
      }
   }
}

void RoadSigns::setUp()
{
}