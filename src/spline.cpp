#include "spline.hpp"

std::vector<glm::vec3> Spline::generatedPath;
bool Spline::splineGenerated = false;

glm::vec3 Spline::catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
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

void Spline::generateSpline()
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
      for (int j = 0; j < samplesPerFragment; ++j)
      {
         float t = static_cast<float>(j) / samplesPerFragment;
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

std::vector<glm::vec3> Spline::generateSpline(std::vector<glm::vec3> pointsList)
{
   std::vector<glm::vec3> generatedPathList;

   // Catmull-Rom spline must have at least 4 points.
   if (pointsList.size() < 4)
   {
      return generatedPathList;
   }

   // Generate Catmull-Rom spline points.
   for (size_t i = 1; i < pointsList.size() - 2; ++i)
   {
      for (int j = 0; j < samplesPerFragment; ++j)
      {
         float t = static_cast<float>(j) / samplesPerFragment;
         glm::vec3 point = catmullRom(pointsList[i - 1],
                                      pointsList[i],
                                      pointsList[i + 1],
                                      pointsList[i + 2],
                                      t);

         generatedPathList.push_back(point);
      }
   }

   return generatedPathList;
}