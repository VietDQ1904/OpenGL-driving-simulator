#include "spline.hpp"
#include <vector>
#include <iostream>
#include <memory>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"
#include "perlinNoise.hpp"
#include "vertices.hpp"
#include "utils.hpp"
#include "noiseMultipliers.hpp"

#ifndef TREE_TERRAIN_IMPLEMENTATION
#define TREE_TERRAIN_IMPLEMENTATION

class Trees : Spline
{
public:
   PerlinNoise noise;
   unsigned int seed = 255;
   std::unique_ptr<Model> treeModel;
   std::unique_ptr<Model> treeLPModel;
   std::unique_ptr<Model> treeLP2Model;

   std::unordered_map<std::array<float, 3>, std::vector<glm::mat4>, ArrayHash<3>, ArrayEqual<3>> modelMatrices1;
   std::vector<GLuint> modelVAOs1;
   std::vector<GLuint> modelLPVAOs1;
   std::vector<GLuint> modelLP2VAOs1;
   std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> modelVBOs1;
   Vertices instances;

   float range = 30.0f;
   float nearRange = pathWidth + offset;
   float farRange = nearRange + range;
   unsigned int spawnChancePercentage = 50;
   const float amplitude = 10.0f;
   const float noiseScale = 0.15f;
   const float grassPathWidth = 40.0f;
   const float offset = 5.0f;
   const int partitionSize = 10;
   const float maxRenderDistance = 300.0f;
   const float mediumRenderDistance = 150.0f;
   const float renderDistance = 50.0f;

   Trees()
   {
      this->generateSpline();
      this->generateModels();
      this->loadResources();
   }
   ~Trees()
   {
      this->unbindBuffers();
   }

   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void loadResources();
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);

private:
   void generateModels();
   void setUp();
   void unbindBuffers();
};

#endif