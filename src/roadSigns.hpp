#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "spline.hpp"
#include "model.hpp"

#ifndef ROAD_SIGN
#define ROAD_SIGN

enum SignTypes
{
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
   RoadSigns()
   {

      this->generateModels();
      this->setUp();
   }

private:
   std::array<std::string, 6> diffuseTextures = {"RoadSignDiffuseA",
                                                 "RoadSignDiffuseB",
                                                 "RoadSignDiffuseC",
                                                 "RoadSignDiffuseD",
                                                 "RoadSignDiffuseE",
                                                 "RoadSignDiffuseF"};

   void generateModels();
   void setUp();
};

#endif