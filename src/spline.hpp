#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SPLINE
#define SPLINE

class Spline{
   protected:
      std::vector<glm::vec3> points = {
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(21.0f, 0.5f ,-3.0f),
         glm::vec3(36.0f, 0.0f, -12.0f),
         glm::vec3(51.0f, 0.5f, -2.0f),
         glm::vec3(65.0f, 0.5f, -5.0f),
         glm::vec3(76.0f, 0.8f, -13.0f),
         glm::vec3(66.0f, 0.9f, -25.0f),
         glm::vec3(73.0f, 1.2f, -38.0f),
         glm::vec3(56.0f, 0.5f, -45.0f),
         glm::vec3(38.0f, 0.5f, -55.0f),
         glm::vec3(31.0f, 0.5f, -41.0f),
         glm::vec3(13.0f, 0.0f, -46.0f),
         glm::vec3(-2.0f, -0.5f, -34.0f),
         glm::vec3(-23.0f, -1.0f, -32.0f),
         glm::vec3(-30.0f, -0.3f, -17.0f),
         glm::vec3(-5.0f, 0.0f, -1.0f), 
      };

      float alpha = 0.5f;
      int samplePerFragments = 20;
      float roadPathWidth = 15.0f;
      float terrainPathWidth = 30.0f;
      std::vector<glm::vec3> generatedPath;

      glm::vec3 catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                           const glm::vec3 &p2, const glm::vec3 &p3,
                           float t){
         auto tj = [](const glm::vec3& a, const glm::vec3& b, float alpha) {
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

         glm::vec3 C  = ((t2 - t_actual) / (t2 - t1)) * B1 + ((t_actual - t1) / (t2 - t1)) * B2;
         
         return C;
      }

      void generateSpline(){
         // Catmull-Rom spline must have at least 4 points.
         if (points.size() < 4){
            return;
         }

         points.insert(points.begin(), points[points.size() - 2]);
         points.push_back(points[1]);
         points.push_back(points[2]);

         // Generate Catmull-Rom spline curve.
         for (size_t i = 1; i < points.size() - 2; ++i){
            for (int j = 0; j < samplePerFragments; ++j){
               float t = static_cast<float>(j) / samplePerFragments;
               glm::vec3 point = catmullRom(points[i - 1],
                                          points[i],
                                          points[i + 1],
                                          points[i + 2], 
                                          t);
               
               generatedPath.push_back(point);
            }
         }
      }
      

};

#endif