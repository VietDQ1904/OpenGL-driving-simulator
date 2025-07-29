#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"

#ifndef ROAD_PATH
#define ROAD_PATH

class Road{
   public:
      std::vector<glm::vec3> points = {
         glm::vec3(-10, 0, 0),
         glm::vec3(-5, -1, 5),
         glm::vec3(0, -2, 0),
         glm::vec3(5, -1, -5),
         glm::vec3(10, 0, 0),
         glm::vec3(15, -1, 5)
      };

      std::vector<float> vertices;
      std::vector<int> indices;
      std::vector<glm::vec3> roadPath;

      GLuint vao, vbo, ebo;
      glm::mat4 model;

      float alpha = 0.5f;
      int samplePerFragments = 20;
      float roadPathWidth = 20.0f;
      float tileLength = 1.0f / roadPathWidth;
      
      Road(){

         this->generateSpline();
         this->generateVertices();
         this->generateIndices();
         
         this->setUp();
      } 

      ~Road(){
         this->cleanUpBuffers();
      }

      void render(Shader &shader);
      void cleanUpBuffers();

   private:
      glm::vec3 catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                           const glm::vec3 &p2, const glm::vec3 &p3,
                           float t);

      void generateSpline();
      void generateIndices();
      void generateVertices();
      void setUp();

};


#endif