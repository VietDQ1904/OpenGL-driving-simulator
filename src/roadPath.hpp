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

#ifndef ROAD_PATH
#define ROAD_PATH

class Road: public Spline{
   public:

      std::vector<float> vertices;
      std::vector<int> indices;

      GLuint vao, vbo, ebo;
      glm::mat4 model;

      float roadPathWidth = pathWidth;
      //float alpha = 0.5f;
      //float roadPathWidth = 15.0f;
      // float tileLength = roadPathWidth;
      
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
      void generateIndices();
      void generateVertices(Physics &simulation);
      void setUp();

};


#endif