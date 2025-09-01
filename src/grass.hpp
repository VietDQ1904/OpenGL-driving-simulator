#include <glad/glad.h>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <random>
#include "model.hpp"
#include "spline.hpp"
#include "camera.hpp"
#include "perlinNoise.hpp"
#include "vertices.hpp"
#include "worleyNoise.hpp"
#include "noiseMultipliers.hpp"

#ifndef GRASS_IMPLEMENTATION
#define GRASS_IMPLEMENTATION

class GrassBlades : Spline
{
public:
   const float grassPathWidth = 40.0f;
   const float grassWidthSize = 16;
   const float grassHeightSize = 60;
   const float renderDistance = 80.0f;
   const float maxRenderDistance = 300.0f;
   const float offset = 5.0f;
   const int partitionSize = 5;
   const float amplitude = 10.0f;
   const float noiseScale = 0.15f;
   const float zScale = 1.25f;
   const unsigned int seed = 255;

   std::unique_ptr<Model> grassBladeModel1;
   std::unique_ptr<Model> grassBladeModel2;
   std::unique_ptr<Model> grassBladeModel3;

   std::unique_ptr<Model> grassBladeModelLP1;
   std::unique_ptr<Model> grassBladeModelLP2;
   std::unique_ptr<Model> grassBladeModelLP3;

   std::vector<GLuint> modelVAOs1, modelVAOs2, modelVAOs3;
   std::vector<GLuint> modelLPVAOs1, modelLPVAOs2, modelLPVAOs3;
   std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> modelVBOs1, modelVBOs2, modelVBOs3;

   std::unordered_map<std::array<float, 3>, std::vector<glm::mat4>, ArrayHash<3>, ArrayEqual<3>> modelMatricesList1;
   std::unordered_map<std::array<float, 3>, std::vector<glm::mat4>, ArrayHash<3>, ArrayEqual<3>> modelMatricesList2;
   std::unordered_map<std::array<float, 3>, std::vector<glm::mat4>, ArrayHash<3>, ArrayEqual<3>> modelMatricesList3;

   ModelInstances modelInstances;

   PerlinNoise noise;
   WorleyNoise grassMap;

   GrassBlades()
   {
      this->generateSpline();
      this->generateGrassModels();
   }

   ~GrassBlades()
   {
      this->unbindBuffers();
   }

   void loadResources();
   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
   void setWindParameters(float time, glm::vec3 windDirection, float windStrength);

private:
   void generateGrassModels();
   void setUp();
   void unbindBuffers();
};

#endif