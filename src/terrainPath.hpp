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

#ifndef TERRAIN_PATH
#define TERRAIN_PATH

struct Triangle{
   glm::vec3 a;
   glm::vec3 b;
   glm::vec3 c;
};

struct Quadrilateral{
   glm::vec3 a;
   glm::vec3 b;
   glm::vec3 c;
   glm::vec3 d;
};

class Terrain: public Spline{
   public:

      std::vector<float> vertices;
      std::vector<int> indices;
      std::vector<float> verticesSub;

      GLuint vao, vbo, ebo;
      GLuint vao2, vbo2;

      glm::mat4 model;

      float alpha = 0.5f;
      int samplePerFragments = 20;
      float tileLength = roadPathWidth;
      int subdivision = 2;
      int horizontalTiles = static_cast<int>(terrainPathWidth / 5.0f);
      
      Terrain(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         this->generateSpline();
         this->generateVertices(simulation);
         this->generateIndices();
         
         this->setUp();
      } 

      ~Terrain(){
         this->cleanUpBuffers();
      }

      void render(Shader &shader);
      void cleanUpBuffers();

   private:
      void generateIndices();
      void subdivide(std::vector<Triangle> &triangles, const Triangle &triangle, int depth);
      void subdivide(std::vector<Quadrilateral> &quadrilaterals, const Quadrilateral &quadrilateral, int depth);
      void generateVertices(Physics &simulation);
      void setUp();

};

#endif