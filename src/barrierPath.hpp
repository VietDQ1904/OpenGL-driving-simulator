#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"
#include "model.hpp"
#include "camera.hpp"

#ifndef BARRIER_PATH
#define BARRIER_PATH

class Barrier: public Spline{
   public:

      GLuint barrierVAO, barrierBuffer;
      glm::mat4 model;
      float barrierOffset = pathWidth / 2.0f + 3.0f;
      float barrierHeight = 1.0f;

      std::vector<std::vector<glm::mat4>> modelMatricesList;
      int partitionSize = 20;
      float renderDistance = 250.0f;
   
      std::vector<glm::vec3> pivots;
      std::vector<GLuint> barrierBuffers;

      glm::mat4 scale;

      Model *barrierModel;

      Barrier(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         barrierModel = new Model("../assets/Barrier/barrier.obj");
         barrierModel->loadShader("barrierShader", "../src/barrier.vert", "../src/barrier.frag", nullptr);

         this->generateSpline();
         this->generateVertices(simulation);
         this->setUp();
         
      } 

      ~Barrier(){
         this->cleanUpBuffers();
         delete barrierModel;
      }

      void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
      void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
      void cleanUpBuffers();

   private:
      void generateVertices(Physics &simulation);
      void setUp();

};

#endif