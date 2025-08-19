#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"
#include "vertices.hpp"
#include "camera.hpp"

#ifndef ROAD_PATH
#define ROAD_PATH

class Road: public Spline{
   public:
      std::vector<float> vertices;
      std::vector<int> indices;

      
      glm::mat4 model;

      float roadPathWidth = pathWidth;
      //float alpha = 0.5f;
      //float roadPathWidth = 15.0f;
      // float tileLength = roadPathWidth;
      VerticesWithIndices verticesWithIndices;
      int partitionSize = 50;
      float renderDistance = 350.0f;
      
      Road(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         this->generateSpline();
         this->generateVertices(simulation);
         
         this->setUp();
      } 

      ~Road(){
         this->cleanUpBuffers();
      }

      void render(Shader &shader, Camera &camera);
      void cleanUpBuffers();

   private:
      void generateIndices();
      void generateVertices(Physics &simulation);
      void setUp();

};


#endif