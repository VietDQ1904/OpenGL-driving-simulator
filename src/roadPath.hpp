#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"
#include "vertices.hpp"
#include "camera.hpp"

#ifndef ROAD_PATH
#define ROAD_PATH

class Road : public Spline
{
public:
   std::vector<float> vertices;
   std::vector<int> indices;

   glm::mat4 model;

   float roadPathWidth = pathWidth;

   VerticesWithIndices verticesWithIndices;
   int partitionSize = 20;
   float renderDistance = 350.0f;

   Road()
   {
      this->generateSpline();
      this->generateVertices();
   }

   ~Road()
   {
      this->cleanUpBuffers();
   }

   void render(Shader &shader, Camera &camera);
   void cleanUpBuffers();
   void addRigidBodies(Physics &simulation);
   void setUp();

private:
   void generateVertices();
};

#endif