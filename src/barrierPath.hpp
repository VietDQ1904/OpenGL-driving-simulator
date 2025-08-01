#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"

#ifndef BARRIER_PATH
#define BARRIER_PATH

class Barrier: public Spline{
   public:

      std::vector<float> vertices;
      std::vector<int> indices;

      GLuint vao, vbo, ebo;
      glm::mat4 model;
      float barrierOffset = roadPathWidth + 5.0f;

      Barrier(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         this->generateSpline();
         this->generateVertices(simulation);
         this->generateIndices();
         this->setUp();
      } 

      ~Barrier(){
         this->cleanUpBuffers();
      }

      void render(Shader &shader);
      void cleanUpBuffers();

   private:
      void generateIndices();
      void generateVertices(Physics &simulation);
      void setUp();

};

#endif