#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SPLINE
#define SPLINE
class Spline
{
protected:
   std::vector<glm::vec3> points = {
       glm::vec3(-62.0f, 0.0f, 8.5f),
       glm::vec3(-43.0f, 0.5f, 9.0f),
       glm::vec3(-23.0f, 1.0f, 10.0f),
       glm::vec3(-4.3f, 1.5f, 10.0f),
       glm::vec3(10.0f, 2.0f, 1.0f),
       glm::vec3(19.8f, 2.0f, -10.0f),
       glm::vec3(33.0f, 2.5f, -16.0f),
       glm::vec3(44.0f, 3.0f, -6.0f),
       glm::vec3(45.0f, 3.5f, 10.0f),
       glm::vec3(50.0f, 4.0f, 25.0f),
       glm::vec3(59.0f, 4.8f, 34.0f),
       glm::vec3(73.0f, 5.6f, 36.0f),
       glm::vec3(87.0f, 6.5f, 28.0f),
       glm::vec3(100.0f, 7.0f, 40.0f),
       glm::vec3(114.0f, 7.5f, 31.0f),
       glm::vec3(123.0f, 8.2f, 19.3f),
       glm::vec3(130.0f, 9.0f, 6.3f)};

   const float alpha = 1.0f;
   const int samplePerFragments = 15;
   const float pathWidth = 15.0f;
   static std::vector<glm::vec3> generatedPath;
   static bool splineGenerated;

   glm::vec3 catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                        const glm::vec3 &p2, const glm::vec3 &p3,
                        float t)
   {
      auto tj = [](const glm::vec3 &a, const glm::vec3 &b, float alpha)
      {
         return std::pow(glm::length(b - a), alpha);
      };

      float t0 = 0.0f;
      float t1 = t0 + tj(p0, p1, alpha);
      float t2 = t1 + tj(p1, p2, alpha);
      float t3 = t2 + tj(p2, p3, alpha);

      // Change t from [0, 1] -> [t1, t2]
      float t_actual = t1 + (t2 - t1) * t;

      glm::vec3 A1 = ((t1 - t_actual) / (t1 - t0)) * p0 + ((t_actual - t0) / (t1 - t0)) * p1;
      glm::vec3 A2 = ((t2 - t_actual) / (t2 - t1)) * p1 + ((t_actual - t1) / (t2 - t1)) * p2;
      glm::vec3 A3 = ((t3 - t_actual) / (t3 - t2)) * p2 + ((t_actual - t2) / (t3 - t2)) * p3;

      glm::vec3 B1 = ((t2 - t_actual) / (t2 - t0)) * A1 + ((t_actual - t0) / (t2 - t0)) * A2;
      glm::vec3 B2 = ((t3 - t_actual) / (t3 - t1)) * A2 + ((t_actual - t1) / (t3 - t1)) * A3;

      glm::vec3 C = ((t2 - t_actual) / (t2 - t1)) * B1 + ((t_actual - t1) / (t2 - t1)) * B2;

      return C;
   }

   void generateSpline()
   {
      for (auto &point : points)
      {
         point *= 5.0f;
      }

      // Catmull-Rom spline must have at least 4 points.
      if (points.size() < 4)
      {
         return;
      }

      // Generate Catmull-Rom spline points.
      for (size_t i = 1; i < points.size() - 2; ++i)
      {
         for (int j = 0; j < samplePerFragments; ++j)
         {
            float t = static_cast<float>(j) / samplePerFragments;
            glm::vec3 point = catmullRom(points[i - 1],
                                         points[i],
                                         points[i + 1],
                                         points[i + 2],
                                         t);

            generatedPath.push_back(point);
         }
      }

      // Only generate spline once
      splineGenerated = true;
   }
};

#endif