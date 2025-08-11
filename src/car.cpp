#include "car.hpp"

Car::Car(Physics& simulation): simulation(simulation){

   this->spawn = glm::vec3(10.0f, 3.0f, 1.0f);
   this->carPos = glm::vec3(0.0f, 1.0f, 0.0f);
   this->carSize = glm::vec3(1.0f, 0.6f, 3.0f);
   this->carRot = glm::vec3(0.0f, 0.0f, 0.0f);
   this->wheelPos_1 = glm::vec3(-1.0f, 0.5f, -2.1f);
   this->wheelSize_1 = glm::vec3(0.4f, 0.35f, 0.35f);
   this->wheelRot_1 = glm::vec3(0.0f, 0.0f, glm::radians(-90.0f));
   this->wheelPos_2 = glm::vec3(1.0f, 0.5f, -2.1f);
   this->wheelSize_2 = glm::vec3(0.4f, 0.35f, 0.35f);
   this->wheelRot_2 = glm::vec3(0.0f, 0.0f, glm::radians(90.0f));
   this->wheelPos_3 = glm::vec3(-1.0f, 0.5f, 1.6f);
   this->wheelSize_3 = glm::vec3(0.45f, 0.4f, 0.4f);
   this->wheelRot_3 = glm::vec3(0.0f, 0.0f, glm::radians(-90.0f));
   this->wheelPos_4 = glm::vec3(1.0f, 0.5f, 1.6f);
   this->wheelSize_4 = glm::vec3(0.45f, 0.4f, 0.4f);
   this->wheelRot_4 = glm::vec3(0.0f, 0.0f, glm::radians(90.0f));

   setUp();
}

void Car::initCarBody(glm::vec3 spawn, glm::vec3 carPos, glm::vec3 carSize, glm::vec3 carRot){
   this->spawn = spawn;
   this->carPos = carPos;
   this->carSize = carSize;
   this->carRot = carRot;
}

void Car::initWheel_1(glm::vec3 wheelPos_1, glm::vec3 wheelSize_1, glm::vec3 wheelRot_1){
   this->wheelPos_1 = wheelPos_1;
   this->wheelSize_1 = wheelSize_1;
   this->wheelRot_1 = wheelRot_1;
}

void Car::initWheel_2(glm::vec3 wheelPos_2, glm::vec3 wheelSize_2, glm::vec3 wheelRot_2){
   this->wheelPos_2 = wheelPos_2;
   this->wheelSize_2 = wheelSize_2;
   this->wheelRot_2 = wheelRot_2;
}

void Car::initWheel_3(glm::vec3 wheelPos_3, glm::vec3 wheelSize_3, glm::vec3 wheelRot_3){
   this->wheelPos_3 = wheelPos_3;
   this->wheelSize_3 = wheelSize_3;
   this->wheelRot_3 = wheelRot_3;
}

void Car::initWheel_4(glm::vec3 wheelPos_4, glm::vec3 wheelSize_4, glm::vec3 wheelRot_4){
   this->wheelPos_4 = wheelPos_4;
   this->wheelSize_4 = wheelSize_4;
   this->wheelRot_4 = wheelRot_4;
}

Car::~Car(){
   carModel->cleanUpBuffers();
   wheelModel1->cleanUpBuffers();
   wheelModel2->cleanUpBuffers();

   delete car, wheel1, wheel2, wheel3, wheel4;
   delete carModel, wheelModel1, wheelModel2;
   delete springConst1, springConst2, springConst3, springConst4;
}

void Car::loadModels(std::string carModelPath, std::string wheelModelPath_1, std::string wheelModelPath_2){

   this->carModel = new Model(carModelPath.c_str());
   this->wheelModel1 = new Model(wheelModelPath_1.c_str());
   this->wheelModel2 = new Model(wheelModelPath_2.c_str());

}

void Car::loadShaderCarBody(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   this->carModel->loadShader(shaderName, vertexFile, fragmentFile, geometryFile);
}

void Car::loadShaderFrontWheels(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   this->wheelModel1->loadShader(shaderName, vertexFile, fragmentFile, geometryFile);
}

void Car::loadShaderBackWheels(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   this->wheelModel2->loadShader(shaderName, vertexFile, fragmentFile, geometryFile);
}

void Car::update(){
   btMatrix3x3 rot = car->getWorldTransform().getBasis();
   short braking = 1;

   float linearVelocity = car->getLinearVelocity().length();

   if (acceleration < 0 && movingForward){
      braking = 0;
   }
   else{
      
      if (linearVelocity < maxVelocity / (1 + 9 * (acceleration < 0))){
         float torque = -maxAcceleration * acceleration * (1 - (std::abs(steering) * (linearVelocity > 10)) / 2);
         wheel1->applyTorque(rot * btVector3(torque, 0, 0));
         wheel2->applyTorque(rot * btVector3(torque, 0, 0));
         if (!handbrake){
            wheel3->applyTorque(rot * btVector3(torque, 0, 0));
            wheel4->applyTorque(rot * btVector3(torque, 0, 0));
         }
      }
   }
   
   springConst1->setAngularLowerLimit(btVector3(braking, wheelSteeringAngle * steering, 0.0));
   springConst1->setAngularUpperLimit(btVector3(-braking, wheelSteeringAngle * steering, 0.0));
   springConst2->setAngularLowerLimit(btVector3(braking, wheelSteeringAngle * steering, 0.0));
   springConst2->setAngularUpperLimit(btVector3(-braking, wheelSteeringAngle * steering, 0.0));

   if (handbrake){
      springConst3->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
      springConst3->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
      springConst4->setAngularLowerLimit(btVector3(0.0, 0.0, 0.0));
      springConst4->setAngularUpperLimit(btVector3(0.0, 0.0, 0.0));
   }
   else{
      springConst3->setAngularLowerLimit(btVector3(braking, 0.0, 0.0));
      springConst3->setAngularUpperLimit(btVector3(-braking, 0.0, 0.0));
      springConst4->setAngularLowerLimit(btVector3(braking, 0.0, 0.0));
      springConst4->setAngularUpperLimit(btVector3(-braking, 0.0, 0.0));
   }

   if (getUp){
      car->applyTorqueImpulse(rot * btVector3(0.0, 0.0, 10000.0));
   }

   if (jump){
      car->applyCentralImpulse(btVector3(0.0, 10000.0, 0.0));
   }

}

void Car::control(GLFWwindow *window, float &deltaTime){

   float steeringLimit = 1.0f;
   float steeringSpeed = 0.05f;

   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      if (steering > -steeringLimit)
         steering -= steeringSpeed;
   } 
   else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      if (steering < steeringLimit)
         steering += steeringSpeed;
   } 
   else {
      steering -= steeringSpeed * ((steering > 0) - (steering < 0));
      if (steering < steeringSpeed && steering > -steeringSpeed)
         steering = 0.0f;
   }

   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      acceleration = 1;
   } 
   else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      acceleration = -1;
   } 
   else {
      acceleration = 0;
      handbrake = true;
   }

   // Car controls - handbrake
   if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      handbrake = true;
   } else {
      handbrake = false;
   }

   // Car controls - get up
   if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !gotUp) {
      getUp = true;
      gotUp = true;
   } 
   else {
      getUp = false;
   }

   if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
      gotUp = false;
   }

   // Car controls - jump upwards
   if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !jumped) {
      jump = true;
      jumped = true;
   } 
   else {
      jump = false;
   }

   if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
      jumped = false;
   }

   // Reverse car
   if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
      acceleration = -1;
      movingForward = false;
   }
   else{
      movingForward = true;
   }

}

void Car::setUp(){
   
   // Car
   car = simulation.createRigidBody(BOX, this->carPos + this->spawn, this->carSize, this->carRot, carMass, 1.75f, 0.2f,
   COLLISION_CHASSIS, COLLISION_ELSE ^ COLLISION_CAR);
   car->setSleepingThresholds(0.0f, 0.0f);
   car->setDamping(carLinearDamp * assist, carAngularDamp * assist);

   // Frame transform
   btTransform frameA, frameB;

   // Wheel 1
   wheel1 = simulation.createRigidBody(CYLINDER, this->wheelPos_1 + this->spawn, 
                                                 this->wheelSize_1, this->wheelRot_1, wheelMass_1,
   wheelFriction, 0.0f, COLLISION_WHEEL, COLLISION_ELSE ^ COLLISION_CAR);
   wheel1->setSleepingThresholds(0.0f, 0.0f);
   wheel1->setDamping(wheelLinearDamp * assist, wheelAngularDamp * assist);
   
   frameA = btTransform::getIdentity();
   frameB = btTransform::getIdentity();

   frameA.getBasis().setEulerZYX(0, 0, 0);
   frameB.getBasis().setEulerZYX(0, 0, glm::radians(-90.0f));
   frameA.setOrigin(btVector3(-this->wheelPos_1.x, -this->wheelPos_1.y, this->wheelPos_1.z));
   frameB.setOrigin(btVector3(0.0, 0.0, 0.0));

   springConst1 = new btGeneric6DofSpringConstraint(*car, *wheel1, frameA, frameB, true);
   springConst1->setLinearLowerLimit(btVector3(0, -lowLimit, 0));
   springConst1->setLinearUpperLimit(btVector3(0, -upLimit, 0));
   springConst1->setAngularLowerLimit(btVector3(1, -0.5, 0));
   springConst1->setAngularUpperLimit(btVector3(-1, 0.5, 0));
   springConst1->enableSpring(1, true);
   springConst1->setStiffness(1, wheelStiffness);
   springConst1->setDamping(1, wheelDamping);
   springConst1->setEquilibriumPoint();

   // Wheel 2
   wheel2 = simulation.createRigidBody(CYLINDER, this->wheelPos_2 + this->spawn, 
                                                 this->wheelSize_2, this->wheelRot_2, wheelMass_1,
   wheelFriction, 0.0f, COLLISION_WHEEL, COLLISION_ELSE ^ COLLISION_CAR);
   wheel2->setSleepingThresholds(0.0f, 0.0f);
   wheel2->setDamping(wheelLinearDamp * assist, wheelAngularDamp * assist);
   
   frameA = btTransform::getIdentity();
   frameB = btTransform::getIdentity();

   frameA.getBasis().setEulerZYX(0, 0, 0);
   frameB.getBasis().setEulerZYX(0, 0, glm::radians(90.0f));
   frameA.setOrigin(btVector3(-this->wheelPos_2.x, -this->wheelPos_2.y, this->wheelPos_2.z));
   frameB.setOrigin(btVector3(0.0, 0.0, 0.0));

   springConst2 = new btGeneric6DofSpringConstraint(*car, *wheel2, frameA, frameB, true);
   springConst2->setLinearLowerLimit(btVector3(0, -lowLimit, 0));
   springConst2->setLinearUpperLimit(btVector3(0, -upLimit, 0));
   springConst2->setAngularLowerLimit(btVector3(1, -0.5, 0));
   springConst2->setAngularUpperLimit(btVector3(-1, 0.5, 0));
   springConst2->enableSpring(1, true);
   springConst2->setStiffness(1, wheelStiffness);
   springConst2->setDamping(1, wheelDamping);
   springConst2->setEquilibriumPoint();

   // Wheel 3
   wheel3 = simulation.createRigidBody(CYLINDER, this->wheelPos_3 + this->spawn, 
                                                 this->wheelSize_3, this->wheelRot_3, wheelMass_2,
   wheelFriction, 0.0f, COLLISION_WHEEL, COLLISION_ELSE ^ COLLISION_CAR);
   wheel3->setSleepingThresholds(0.0f, 0.0f);
   wheel3->setDamping(wheelLinearDamp * assist, wheelAngularDamp * assist);
   
   frameA = btTransform::getIdentity();
   frameB = btTransform::getIdentity();

   frameA.getBasis().setEulerZYX(0, 0, 0);
   frameB.getBasis().setEulerZYX(0, 0, glm::radians(90.0f));
   frameA.setOrigin(btVector3(this->wheelPos_3.x, -this->wheelPos_3.y, this->wheelPos_3.z));
   frameB.setOrigin(btVector3(0.0, 0.0, 0.0));

   springConst3 = new btGeneric6DofSpringConstraint(*car, *wheel3, frameA, frameB, true);
   springConst3->setLinearLowerLimit(btVector3(0, -lowLimit, 0));
   springConst3->setLinearUpperLimit(btVector3(0, -upLimit, 0));
   springConst3->setAngularLowerLimit(btVector3(1, 0, 0));
   springConst3->setAngularUpperLimit(btVector3(-1, 0, 0));
   springConst3->enableSpring(1, true);
   springConst3->setStiffness(1, wheelStiffness);
   springConst3->setDamping(1, wheelDamping);
   springConst3->setEquilibriumPoint();

   // Wheel 4
   wheel4 = simulation.createRigidBody(CYLINDER, this->wheelPos_4 + this->spawn, 
                                                 this->wheelSize_4, this->wheelRot_4, wheelMass_2,
   wheelFriction, 0.0f, COLLISION_WHEEL, COLLISION_ELSE ^ COLLISION_CAR);
   wheel4->setSleepingThresholds(0.0f, 0.0f);
   wheel4->setDamping(wheelLinearDamp * assist, wheelAngularDamp * assist);
   
   frameA = btTransform::getIdentity();
   frameB = btTransform::getIdentity();

   frameA.getBasis().setEulerZYX(0, 0, 0);
   frameB.getBasis().setEulerZYX(0, 0, glm::radians(-90.0f));
   frameA.setOrigin(btVector3(this->wheelPos_4.x, -this->wheelPos_4.y, this->wheelPos_4.z));
   frameB.setOrigin(btVector3(0.0, 0.0, 0.0));

   springConst4 = new btGeneric6DofSpringConstraint(*car, *wheel4, frameA, frameB, true);
   springConst4->setLinearLowerLimit(btVector3(0, -lowLimit, 0));
   springConst4->setLinearUpperLimit(btVector3(0, -upLimit, 0));
   springConst4->setAngularLowerLimit(btVector3(1, 0, 0));
   springConst4->setAngularUpperLimit(btVector3(-1, 0, 0));
   springConst4->enableSpring(1, true);
   springConst4->setStiffness(1, wheelStiffness);
   springConst4->setDamping(1, wheelDamping);
   springConst4->setEquilibriumPoint();

   simulation.dynamicsWorld->addConstraint(springConst1);
   simulation.dynamicsWorld->addConstraint(springConst2);
   simulation.dynamicsWorld->addConstraint(springConst3);
   simulation.dynamicsWorld->addConstraint(springConst4);
}

void Car::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor){
   carModel->modelShader.use();
   carModel->modelShader.setVec3("light.direction", direction);
   carModel->modelShader.setVec3("light.color", lightColor);

   wheelModel1->modelShader.use();
   wheelModel1->modelShader.setVec3("light.direction", direction);
   wheelModel1->modelShader.setVec3("light.color", lightColor);

   wheelModel2->modelShader.use();
   wheelModel2->modelShader.setVec3("light.direction", direction);
   wheelModel2->modelShader.setVec3("light.color", lightColor);
}

void Car::render(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos){
   
   carModel->modelShader.use();
   car->getMotionState()->getWorldTransform(transform);
   transform.getOpenGLMatrix(matrix);
   objectModelMatrix = glm::make_mat4(matrix);
   carModel->modelShader.setMat4("model", objectModelMatrix);
   carModel->modelShader.setMat4("view", view);
   carModel->modelShader.setMat4("projection", projection);
   carModel->modelShader.setVec3("viewPos", viewPos);
   carModel->draw();

   wheelModel1->modelShader.use();
   wheel1->getMotionState()->getWorldTransform(transform);
   transform.getOpenGLMatrix(matrix);
   objectModelMatrix = glm::make_mat4(matrix);
   wheelModel1->modelShader.setMat4("model", objectModelMatrix);
   wheelModel1->modelShader.setMat4("view", view);
   wheelModel1->modelShader.setMat4("projection", projection);
   wheelModel1->modelShader.setVec3("viewPos", viewPos);
   wheelModel1->draw();

   wheelModel1->modelShader.use();
   wheel2->getMotionState()->getWorldTransform(transform);
   transform.getOpenGLMatrix(matrix);
   objectModelMatrix = glm::make_mat4(matrix);
   wheelModel1->modelShader.setMat4("model", objectModelMatrix);
   wheelModel1->modelShader.setMat4("view", view);
   wheelModel1->modelShader.setMat4("projection", projection);
   wheelModel1->modelShader.setVec3("viewPos", viewPos);
   wheelModel1->draw();

   wheelModel2->modelShader.use();
   wheel3->getMotionState()->getWorldTransform(transform);
   transform.getOpenGLMatrix(matrix);
   objectModelMatrix = glm::make_mat4(matrix);
   wheelModel2->modelShader.setMat4("model", objectModelMatrix);
   wheelModel2->modelShader.setMat4("view", view);
   wheelModel2->modelShader.setMat4("projection", projection);
   wheelModel2->modelShader.setVec3("viewPos", viewPos);
   wheelModel2->draw();

   wheelModel2->modelShader.use();
   wheel4->getMotionState()->getWorldTransform(transform);
   transform.getOpenGLMatrix(matrix);
   objectModelMatrix = glm::make_mat4(matrix);
   wheelModel2->modelShader.setMat4("model", objectModelMatrix);
   wheelModel2->modelShader.setMat4("view", view);
   wheelModel2->modelShader.setMat4("projection", projection);
   wheelModel2->modelShader.setVec3("viewPos", viewPos);
   wheelModel2->draw();
}

