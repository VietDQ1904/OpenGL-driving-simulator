#include <iostream>
#include <glad/glad.h>
#include "shader.hpp"
#include "physics.hpp"
#include "spline.hpp"
#include "camera.hpp"
#include "perlinNoise.hpp"
#include "vertices.hpp"

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
      VerticesWithLOD verticesTerrainMaps;
      VerticesWithIndices verticesPathMaps;

      glm::mat4 model;

      float terrainPathWidth = 30.0f;
      float alpha = 0.5f;
      int samplePerFragments = 20;
      float tileLength = pathWidth;
      int subdivision = 3;
      int horizontalTiles = static_cast<int>(terrainPathWidth / 5.0f);
      int partitionSize = 20;
      float renderDistance = 100.0f;
      float maxRenderDistance = 350.0f;

      PerlinNoise noise;
      float amplitude = 10.0f;
      float noiseScale = 0.15f;

      Terrain(Physics &simulation){
         for (auto& point : points) {
            point *= 5.0f;
         }

         this->generateSpline();
         this->generateVertices(simulation);
         this->setUp();
      } 

      ~Terrain(){
         this->cleanUpBuffers();
      }

      void render(Shader &shader, Camera &camera);
      void cleanUpBuffers();

   private:
      void subdivide(std::vector<Triangle> &triangles, const Triangle &triangle, int depth);
      void subdivide(std::vector<Quadrilateral> &quadrilaterals, const Quadrilateral &quadrilateral, int depth);
      void generateVertices(Physics &simulation);
      void setUp();

};

#endif