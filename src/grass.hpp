#include <glad/glad.h>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "model.hpp"
#include "spline.hpp"
#include "camera.hpp"
#include "perlinNoise.hpp"
#include "vertices.hpp"
#include "noiseMultipliers.hpp"

#ifndef GRASS_IMPLEMENTATION
#define GRASS_IMPLEMENTATION

class GrassBlades : Spline
{
public:
   const float grassPathWidth = 30.0f;
   const float grassWidthSize = 16;
   const float grassHeightSize = 60;
   const float renderDistance = 100.0f;
   const float maxRenderDistance = 300.0f;
   const float offset = 5.0f;
   const int partitionSize = 5;
   const float amplitude = 10.0f;
   const float noiseScale = 0.15f;

   std::unique_ptr<Model> grassBladeModel1;
   std::unique_ptr<Model> grassBladeModel2;
   std::unique_ptr<Model> grassBladeModel3;

   std::unique_ptr<Model> grassBladeModelLP1;
   std::unique_ptr<Model> grassBladeModelLP2;
   std::unique_ptr<Model> grassBladeModelLP3;

   std::vector<GLuint> grassVAOs, grassLPVAOs;
   ModelInstances modelInstances1;

   PerlinNoise noise;

   GrassBlades()
   {

      grassBladeModel1 = std::make_unique<Model>("../assets/Grass/grassBladeA.obj");
      grassBladeModel2 = std::make_unique<Model>("../assets/Grass/grassBladeB.obj");
      grassBladeModel3 = std::make_unique<Model>("../assets/Grass/grassBladeC.obj");

      grassBladeModel1->loadShader("GrassBladeShader1",
                                   "../src/Shaders/grassModel.vert",
                                   "../src/Shaders/instanceModel.frag",
                                   nullptr);

      grassBladeModelLP1 = std::make_unique<Model>("../assets/Grass/grassBladeA.obj");
      grassBladeModelLP2 = std::make_unique<Model>("../assets/Grass/grassBladeB.obj");
      grassBladeModelLP3 = std::make_unique<Model>("../assets/Grass/grassBladeC.obj");

      grassBladeModelLP1->loadShader("GrassBladeShaderLP1",
                                     "../src/Shaders/grassModel.vert",
                                     "../src/Shaders/instanceModel.frag",
                                     nullptr);

      if (!splineGenerated)
      {
         this->generateSpline();
      }

      this->generateGrassModels();
      this->setUp();
   }

   ~GrassBlades()
   {
      this->unbindBuffers();
   }

   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
   void setWindParameters(float time, glm::vec3 windDirection, float windStrength);

private:
   void generateGrassModels();
   void setUp();
   void unbindBuffers();
};

#endif