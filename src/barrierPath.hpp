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
   float barrierOffset = (pathWidth / 2.0f);
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

   Barrier()
   {
      this->generateSpline();
      this->offsetPaths();

      // Create two parallel paths.
      generatedLeftPath = this->generateSpline(leftPoints);
      generatedRightPath = this->generateSpline(rightPoints);

      this->generateVertices();
   }

   ~Barrier()
   {
      this->cleanUpBuffers();
      barrierModel->cleanUpBuffers();
      barrierLPModel->cleanUpBuffers();
   }

   void loadResources();
   void addRigidBodies(Physics &simulation);
   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
   void cleanUpBuffers();

private:
   void generateVertices();
   void setUp();
   void offsetPaths();
};

#endif