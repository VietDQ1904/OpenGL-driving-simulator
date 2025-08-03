#include "physics.hpp"
#include "model.hpp"

#ifndef CAR_PHYSICS
#define CAR_PHYSICS


class Car{
   public:
      short acceleration = 0;
      float steering = 0.0f;
      float maxAcceleration = 1200.0f;
      float maxVelocity = 120.0f;
      float carMass = 3000.0f;
      float wheelMass_1 = 15.0f;
      float wheelMass_2 = 20.0f;
      float wheelFriction = 4.25f;
      float wheelStiffness = 220000.0f;
      float wheelDamping = 0.00002f;
      float wheelSteeringAngle = 0.9f;
      float assist = 0.5f;
      float lowLimit = -0.2f;
      float upLimit = 0.2f;
      bool movingForward = true;

      const float carLinearDamp = 0.02f;
      const float carAngularDamp = 0.4f;
      const float wheelLinearDamp = 0.01f;
      const float wheelAngularDamp = 0.2f;

      Physics& simulation;
      btRigidBody *car, *wheel1, *wheel2, *wheel3, *wheel4;
      btGeneric6DofSpringConstraint *springConst1, *springConst2, *springConst3, *springConst4;
      Model *carModel, *wheelModel1, *wheelModel2;

      glm::vec3 spawn, carPos, carSize, carRot;
      glm::vec3 wheelPos_1, wheelPos_2, wheelPos_3, wheelPos_4;
      glm::vec3 wheelSize_1, wheelSize_2, wheelSize_3, wheelSize_4;
      glm::vec3 wheelRot_1, wheelRot_2, wheelRot_3, wheelRot_4;

      bool handbrake = false;
      bool getUp = false, gotUp = false;
      bool jump = false, jumped = false;

      Car(Physics& simulation);
      ~Car();
      void loadModels(std::string carModelPath, std::string wheelModelPath_1, std::string wheelModelPath_2);
      void initCarBody(glm::vec3 spawn, glm::vec3 carPos, glm::vec3 carSize, glm::vec3 carRot);
      void initWheel_1(glm::vec3 wheelPos_1, glm::vec3 wheelSize_1, glm::vec3 wheelRot_1);
      void initWheel_2(glm::vec3 wheelPos_2, glm::vec3 wheelSize_2, glm::vec3 wheelRot_2);
      void initWheel_3(glm::vec3 wheelPos_3, glm::vec3 wheelSize_3, glm::vec3 wheelRot_3);
      void initWheel_4(glm::vec3 wheelPos_4, glm::vec3 wheelSize_4, glm::vec3 wheelRot_4);
      
      void render(Shader shader);
      void update();
      void control(GLFWwindow *window, float &deltaTime);

   private:

      glm::mat4 objectModelMatrix;
      GLfloat matrix[16];
      btTransform transform;

      void setUp();
   
};

#endif