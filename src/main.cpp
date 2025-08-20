#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "cubemap.hpp"
#include "physics.hpp"
#include "car.hpp"
#include "texture.hpp"
#include "resourceManagement.hpp"
#include "roadPath.hpp"
#include "terrainPath.hpp"
#include "barrierPath.hpp"
#include "../lib/stb_image.h"

const float windowWidth = 1080.0f;
const float windowHeight = 720.0f;

// // enable NVIDIA GPU rendering
extern "C" {
   __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// // enable AMD GPU rendering
// extern "C" {
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
// }

void frameBufferSizeCallback(GLFWwindow *window, int width, int height){
   glViewport(0, 0, width, height);
}

// Check if the escape key is pressed, exit when pressed
void processInput(GLFWwindow *window, float &deltaTime){
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
      glfwSetWindowShouldClose(window, true);
   }
}


int main(int argc, char* argv[]){
   glfwInit();
   
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_SAMPLES, 4); // Add 4 subsamples for smoother edges. 

   GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "DrivingGame", nullptr, nullptr);

   if (window == nullptr){
      std::cout << "OPENGL::CANNOT_INITIALIZE_GLFW\n";
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
      std::cout << "OPENGL::CANNOT_INITIALIZE_GLAD.\n";
      return -1;
   }

   glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
   
   glm::mat4 model = glm::mat4(0.0f);
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection;

   glEnable(GL_DEPTH_TEST);

   float deltaTime = 0.0f;
   float lastTime = 0.0f;
   const float radius = 10.0f;

   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   ResourceManagement::loadShader("Main", "../src/model.vert", "../src/model.frag", nullptr);
   ResourceManagement::loadTexture("Grass", "../assets/grass.png", false, false);
   ResourceManagement::loadTexture("Asphalt", "../assets/asphalt.png", false, false);
   ResourceManagement::loadShader("Cubemap", "../src/cubemap.vert", "../src/cubemap.frag", nullptr);

   std::array<std::string, 6> images = {"right.jpg",
                                        "left.jpg", 
                                        "top.jpg", 
                                        "bottom.jpg", 
                                        "front.jpg",
                                        "back.jpg"};

   Cubemap skyBox("../assets/Skybox/", images);

   Physics simulation;
   Road *road = new Road(simulation);
   Terrain *terrain = new Terrain(simulation);
   Barrier *barrier = new Barrier(simulation);
   barrier->setEnvironmentLighting(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

   Car *car = new Car(simulation);
   car->loadModels("../assets/Car/CarBodyModel.obj", "../assets/Car/wheelModel.obj", "../assets/Car/wheelModel.obj");
   car->loadShaderCarBody("CarBodyShader", "../src/modelTexture.vert", "../src/modelTexture.frag", nullptr);
   car->loadShaderFrontWheels("CarFrontWheelsShader", "../src/modelTexture.vert", "../src/modelTexture.frag", nullptr);
   car->loadShaderBackWheels("CarBackWheelsShader", "../src/modelTexture.vert", "../src/modelTexture.frag", nullptr);
   car->setEnvironmentLighting(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

   Camera camera(glm::vec3(-1.0f, 1.0f, 0.0f));
   camera.setPositionToCar(car);
   
   Shader mainShader = ResourceManagement::getShader("Main");
   mainShader.use();

   Shader cubemapShader = ResourceManagement::getShader("Cubemap");
   cubemapShader.use();

   Model roadSign("../assets/RoadSign/roadSignTest.obj");
   roadSign.loadShader("Test", "../src/modelTexture.vert", "../src/modelTexture.frag", nullptr);

   // glfwSetCursorPosCallback(window, mouseCallback); // When move the mouse
   // glfwSetScrollCallback(window, scrollCallback); // When scroll the mouse

   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      
   glfwSetWindowUserPointer(window, &camera);
   glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y){
      if (Camera *cam = static_cast<Camera*> (glfwGetWindowUserPointer(window))){
         cam->mouseCallback(window, x, y);
      }
   });
   
   glfwSetScrollCallback(window, [](GLFWwindow *window, double scroll_x, double scroll_y){
      if (Camera *cam = static_cast<Camera*> (glfwGetWindowUserPointer(window))){
         cam->scrollCallback(window, scroll_x, scroll_y);
      }
   });
   
   float maxSecPerFrame = 1.0f / 60.0f;

   while (!glfwWindowShouldClose(window)){
      
      float timeValue = glfwGetTime();
      deltaTime = timeValue - lastTime;
      lastTime = timeValue; 
      
      processInput(window, deltaTime);
   
      camera.updateFollowCamera(car->car);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
      
      simulation.dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime: maxSecPerFrame), 10);

      projection = camera.getProjectionMatrix(static_cast<float>(windowWidth / windowHeight));
      view = camera.getViewMatrix();
      
      camera.calculateFrustrumPlanes(projection * view, static_cast<float>(windowWidth / windowHeight));

      mainShader.use();

      glActiveTexture(GL_TEXTURE0);
      ResourceManagement::getTexture("Grass").bindTexture();
      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);  
      mainShader.setInt("texture_diffuse1", 0);
      terrain->render(mainShader, camera);

      glActiveTexture(GL_TEXTURE1);
      ResourceManagement::getTexture("Asphalt").bindTexture();
      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);  
      mainShader.setInt("texture_diffuse1", 1);
      road->render(mainShader, camera);
      
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
      roadSign.modelShader.use();
      roadSign.modelShader.setVec3("light.direction", glm::vec3(1.0f, 1.0f, 1.0f));
      roadSign.modelShader.setVec3("light.color", glm::vec3(1.0f, 1.0f, 1.0f));
      roadSign.modelShader.setMat4("view", view);
      roadSign.modelShader.setMat4("projection", projection); 
      roadSign.modelShader.setMat4("model", model);
      roadSign.draw();
      model = glm::mat4(1.0f);

      car->control(window, deltaTime);
      car->update();
      car->render(view, projection, camera.cameraPos);

      barrier->render(view, projection, camera);
      skyBox.draw(cubemapShader, projection, view);
      
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   delete car;
   delete terrain;
   delete road;
   delete barrier;
   skyBox.unbind();
   ResourceManagement::clearResources();

   glfwTerminate();

   return 0;
}
