#include <vector>
#include <iostream>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "vertices.hpp"

#ifndef BARRIER_PATH
#define BARRIER_PATH

class Barrier : public Spline
{
public:
   float barrierOffset = (pathWidth / 2.0f + 2.0f) / 5.0f;
   float barrierHeight = 1.0f;

   int partitionSize = 20;
   float renderDistance = 75.0f;
   float maxRenderDistance = 250.0f;
   float modelScale = 4.0f;

   std::vector<GLuint> barrierVAOs, barrierLPVAOs;
   ModelInstances modelInstances;

   std::unique_ptr<Model> barrierModel;
   std::unique_ptr<Model> barrierLPModel;

   std::vector<glm::vec3> leftPoints;
   std::vector<glm::vec3> rightPoints;

   std::vector<glm::vec3> generatedLeftPath;
   std::vector<glm::vec3> generatedRightPath;

   Barrier(Physics &simulation)
   {
      barrierModel = std::make_unique<Model>("../assets/Barrier/Model2/BarrierModel.obj");
      barrierModel->loadShader("barrierShader", "../src/Shaders/instanceModel.vert", "../src/Shaders/instanceModel.frag", nullptr);
      barrierLPModel = std::make_unique<Model>("../assets/Barrier/Model2/BarrierModel.obj");
      barrierLPModel->loadShader("barrierLPShader", "../src/Shaders/instanceModel.vert", "../src/Shaders/instanceModel.frag", nullptr);

      if (!splineGenerated)
      {
         this->generateSpline();
      }

      this->offsetPaths();

      for (auto &point : leftPoints)
      {
         point *= 5.0f;
      }

      for (auto &point : rightPoints)
      {
         point *= 5.0f;
      }

      // Create two parallel paths.
      generatedLeftPath = this->generateSpline(leftPoints);
      generatedRightPath = this->generateSpline(rightPoints);

      this->generateVertices(simulation);
      this->setUp();
   }

   ~Barrier()
   {
      this->cleanUpBuffers();
      barrierModel->cleanUpBuffers();
      barrierLPModel->cleanUpBuffers();
   }

   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
   void cleanUpBuffers();

private:
   void generateVertices(Physics &simulation);
   void setUp();
   void offsetPaths();
};

#endif