#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "cubemap.hpp"
#include "physics.hpp"
#include "car.hpp"
#include "texture.hpp"
//#include "terrain.hpp"
#include "roadPath.hpp"
#include "terrainPath.hpp"
#include "barrierPath.hpp"
#include "../lib/stb_image.h"

const float windowWidth = 1080.0f;
const float windowHeight = 720.0f;

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

   GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "TestOpenGL", nullptr, nullptr);

   if (window == nullptr){
      std::cout << "Cannot create window.\n";
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
      std::cout << "Cannot initialize glad.\n";
      return -1;
   }

   glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
   GLFWimage imageIcons[1];
   imageIcons[0].pixels = stbi_load("../assets/racing-car.png", &imageIcons[0].width, &imageIcons[0].height, 0, 4);
   glfwSetWindowIcon(window, 1, imageIcons);
   stbi_image_free(imageIcons[0].pixels);
   
   glm::mat4 model = glm::mat4(0.0f);
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection;

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_MULTISAMPLE);

   float deltaTime = 0.0f;
   float lastTime = 0.0f;
   const float radius = 10.0f;


   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   Shader mainShader("../src/model.vert", "../src/model.frag", nullptr);
   Shader cubemapShader("../src/cubemap.vert", "../src/cubemap.frag", nullptr);
   Shader carShader("../src/model.vert", "../src/modelTexture.frag", nullptr);
   Shader barrierShader("../src/barrier.vert", "../src/modelTexture.frag", nullptr);

   Physics simulation;

   Car *car = new Car(simulation);
   car->loadModels("../assets/car.obj", "../assets/wheelFront.obj", "../assets/wheelBack.obj");
   

   std::array<std::string, 6> images = {"right.jpg",
                                        "left.jpg", 
                                        "top.jpg", 
                                        "bottom.jpg", 
                                        "front.jpg",
                                        "back.jpg"};

   Cubemap skyBox("../assets/Skybox/", images);
   
   mainShader.use();

   Model grass("../assets/grass.obj");
   Model asphalt("../assets/asphalt.obj");

   Camera camera(glm::vec3(-1.0f, 1.0f, 0.0f));
   camera.setPositionToCar(car);

   Road *road = new Road(simulation);
   Terrain *terrain = new Terrain(simulation);
   Barrier *barrier = new Barrier(simulation);
   
   glActiveTexture(GL_TEXTURE0);
   Texture texture1("", "../assets/mars.png");
   glActiveTexture(GL_TEXTURE1);
   Texture texture2("", "../assets/concrete.png");
   glActiveTexture(GL_TEXTURE2);
   Texture texture3("", "../assets/grass.png");
   glActiveTexture(GL_TEXTURE3);
   Texture texture4("", "../assets/asphalt.png");
   glActiveTexture(GL_TEXTURE4);
   Texture texture5("", "../assets/car.png");
   glActiveTexture(GL_TEXTURE5);
   Texture texture6("", "../assets/stripeBarrier.png");

   //Terrain terrain;

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
   
   GLfloat maxSecPerFrame = 1.0f / 60.0f;

   while (!glfwWindowShouldClose(window)){
      
      float timeValue = glfwGetTime();
      deltaTime = timeValue - lastTime;
      lastTime = timeValue; 
      
      processInput(window, deltaTime);
      car->control(window, deltaTime);
   
      camera.updateFollowCamera(car->car);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 

      car->update();
      simulation.dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime: maxSecPerFrame), 10);

      projection = glm::perspective(glm::radians(camera.zoom), (float) windowWidth / windowHeight, 0.1f, 200.0f);
      view = glm::mat4(1.0f);
      view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.up);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
      model = glm::scale(model, glm::vec3(1.0f));

      mainShader.use();

      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);  
      mainShader.setInt("texture_diffuse1", 2);
      terrain->render(mainShader, camera);

      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);  
      mainShader.setInt("texture_diffuse1", 3);
      road->render(mainShader);

      carShader.use();
      carShader.setMat4("view", view);
      carShader.setMat4("projection", projection);  
      carShader.setInt("texture_diffuse1", 4);
      car->render(carShader);

      barrierShader.use();
      barrierShader.setMat4("view", view);
      barrierShader.setMat4("projection", projection);  
      barrierShader.setInt("texture_diffuse1", 5);
      barrier->render(barrierShader);

      skyBox.draw(cubemapShader, projection, view);
      
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   delete car, terrain, road, barrier;
   glfwTerminate();

   return 0;
}
