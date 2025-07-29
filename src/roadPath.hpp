#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "physics.hpp"

#ifndef ROAD_PATH
#define ROAD_PATH

class Road{
   public:
      std::vector<glm::vec3> points = {
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(21.0f, 0.5f ,-3.0f),
         glm::vec3(36.0f, 0.0f, -12.0f),
         glm::vec3(51.0f, 0.5f, -2.0f),
         glm::vec3(65.0f, 0.5f, -5.0f),
         glm::vec3(76.0f, 0.8f, -13.0f),
         glm::vec3(66.0f, 0.9f, -25.0f),
         glm::vec3(73.0f, 1.0f, -38.0f),
         glm::vec3(56.0f, 0.5f, -45.0f),
         glm::vec3(38.0f, 0.5f, -55.0f),
         glm::vec3(31.0f, 0.5f, -41.0f),
         glm::vec3(13.0f, 0.0f, -46.0f),
         glm::vec3(-2.0f, -0.5f, -34.0f),
         glm::vec3(-23.0f, -0.2f, -32.0f),
         glm::vec3(-30.0f, 0.0f, -17.0f),
         glm::vec3(-20.0f, 0.0f, -1.0f),
         //glm::vec3(0.0f, 0.0f, 0.0f)
      };


      std::vector<float> vertices;
      std::vector<int> indices;
      std::vector<glm::vec3> roadPath;

      GLuint vao, vbo, ebo;
      glm::mat4 model;

      float alpha = 0.5f;
      int samplePerFragments = 20;
      float roadPathWidth = 20.0f;
      float tileLength = roadPathWidth;
      
      Road(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         this->generateSpline();
         this->generateVertices(simulation);
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
      void generateVertices(Physics &simulation);
      void setUp();

};


#endif