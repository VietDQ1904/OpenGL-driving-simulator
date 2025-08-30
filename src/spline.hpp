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
   const int samplesPerFragment = 15;
   const float pathWidth = 15.0f;
   static std::vector<glm::vec3> generatedPath;
   static bool splineGenerated;

   glm::vec3 catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                        const glm::vec3 &p2, const glm::vec3 &p3,
                        float t);
   void generateSpline();
   std::vector<glm::vec3> generateSpline(std::vector<glm::vec3> pointsList);
};

#endif