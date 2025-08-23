#include <vector>
#include <iostream>
#include <map>
#include <memory>
#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "spline.hpp"
#include "model.hpp"
#include "vertices.hpp"
#include "camera.hpp"

#ifndef ROAD_SIGN
#define ROAD_SIGN

enum SignTypes
{
   STRAIGHT = -1,
   LEFT_CURVE = 0,
   RIGHT_CURVE = 1,
   LEFT_RIGHT_CURVE = 2,
   RIGHT_LEFT_CURVE = 3,
   SHARP_LEFT_CURVE = 4,
   SHARP_RIGHT_CURVE = 5
};

class RoadSigns : Spline
{
public:
   ModelInstances modelInstances;
   std::vector<GLuint> signVAOs, signLPVAOs;
   std::map<std::array<float, 3>, int> directionsMap;
   std::unique_ptr<Model> signModel;
   std::unique_ptr<Model> signModelLP;

   float offset = 5.0f;
   float maxRenderDistance = 300.0f;
   float renderDistance = 200.0f;

   RoadSigns()
   {
      this->generateSpline();

      signModel = std::make_unique<Model>("../assets/RoadSign/roadSign.obj");
      signModelLP = std::make_unique<Model>("../assets/RoadSign/roadSign.obj");
      this->generateModels();
      this->setUp();
   }
   ~RoadSigns()
   {
      this->cleanUpBuffers();
   }
   void render(glm::mat4 view, glm::mat4 projection, Camera &camera);
   void setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor);
   void cleanUpBuffers();

private:
   std::array<std::string, 6> diffuseTextures = {"../assets/RoadSign/RoadSignDiffuseA.png",
                                                 "../assets/RoadSign/RoadSignDiffuseB.png",
                                                 "../assets/RoadSign/RoadSignDiffuseC.png",
                                                 "../assets/RoadSign/RoadSignDiffuseD.png",
                                                 "../assets/RoadSign/RoadSignDiffuseE.png",
                                                 "../assets/RoadSign/RoadSignDiffuseF.png"};

   void generateModels();
   void setUp();
};

#endif