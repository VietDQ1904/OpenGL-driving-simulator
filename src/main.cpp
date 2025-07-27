#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "cubemap.hpp"
#include "physics.hpp"
#include "car.hpp"
#include "texture.hpp"
#include "terrain.hpp"

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
   Shader terrainShader("../src/terrain.vert", "../src/terrain.frag", nullptr);

   Physics simulation;

   const unsigned int gridWidth = 5;
   const unsigned int gridHeight = 8;
   const unsigned int tiles = gridWidth * gridHeight;

   unsigned int track[gridHeight][gridWidth] = {
      {0, 0, 0, 0, 0},
      {0, 1, 1, 1, 0},
      {0, 1, 0, 1, 0},
      {0, 1, 0, 1, 0},
      {0, 1, 0, 1, 0},
      {0, 1, 0, 1, 0},
      {0, 1, 1, 1, 0},
      {0, 0, 0, 0, 0}
   };

   btRigidBody *plane[tiles];
   glm::vec3 planePos[tiles];
   glm::vec3 planeSize[tiles];
   const float planeEdge = 20.0f;

   // Grass & asphalt floor
   for (unsigned int i = 0; i < gridWidth; ++i){
      for (unsigned int j = 0; j < gridHeight; ++j){
         planePos[i * gridHeight + j] = glm::vec3(2* planeEdge * i - planeEdge * (gridWidth - 1), 
                                                 -0.0f * (i * gridHeight + j), 
                                                  2 * planeEdge * j - planeEdge * (gridHeight - 1));
         planeSize[i* gridHeight + j] = glm::vec3(planeEdge, 0.0f, planeEdge);

         glm::vec3 plane_rot = glm::vec3(0.0f, 0.0f, 0.0f);
         if (track[j][i] == 0) {
            // Grass
            plane[i * gridHeight + j] = simulation.createRigidBody(
                                                            BOX, 
                                                            planePos[i * gridHeight + j], 
                                                            planeSize[i * gridHeight + j], 
                                                            plane_rot, 0.0f, 0.25f, 0.25f, 
                                                            COLLISION_TERRAIN, 
                                                            COLLISION_ELSE
                                                         );
         } 
         
         else if (track[j][i] == 1) {
            // Asphalt
            plane[i * gridHeight + j] = simulation.createRigidBody(
                                                            BOX, 
                                                            planePos[i * gridHeight + j] + glm::vec3(0.0f, 0.05f, 0.0f), 
                                                            planeSize[i * gridHeight + j] + glm::vec3(0.0f, 0.05f, 0.0f), 
                                                            plane_rot, 0.0f, 0.5f, 0.5f, 
                                                            COLLISION_TERRAIN, 
                                                            COLLISION_ELSE
                                                         );
         }
      }
   }

   // // Invisible walls
   // const unsigned int walls = 4;
   // float side;
   // glm::vec3 wallPos;
   // glm::vec3 wallSize;
   // btRigidBody *wall;

   // side = planeEdge * gridHeight;
   // wallSize = glm::vec3(2*side, 5.0f, 0.0f);

   // wallPos = glm::vec3(0.0f, 2.5f, -side);
   // wall = simulation.createRigidBody(BOX, wallPos, wallSize, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 
   //                                   COLLISION_TERRAIN, 
   //                                   COLLISION_ELSE);
   // wallPos = glm::vec3(0.0f, 2.5f, side);
   // wall = simulation.createRigidBody(BOX, wallPos, wallSize, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 
   //                                   COLLISION_TERRAIN, 
   //                                   COLLISION_ELSE);

   // side = planeEdge * gridWidth;
   // wallSize = glm::vec3(0.0f, 5.0f, 2*side);

   // wallPos = glm::vec3(-side, 2.5f, 0.0f);
   // wall = simulation.createRigidBody(BOX, wallPos, wallSize, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 
   //                                   COLLISION_TERRAIN, 
   //                                   COLLISION_ELSE);
   // wallPos = glm::vec3(side, 2.5f, 0.0f);
   // wall = simulation.createRigidBody(BOX, wallPos, wallSize, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 
   //                                   COLLISION_TERRAIN, 
   //                                   COLLISION_ELSE);


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

   Model planet("../assets/planet.obj");
   Model baseplate("../assets/baseplate.obj");
   Model grass("../assets/grass.obj");
   Model asphalt("../assets/asphalt.obj");

   Camera camera(glm::vec3(-1.0f, 1.0f, 0.0f));
   camera.setPositionToCar(car);

   
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

   Terrain terrain;

   // glfwSetCursorPosCallback(window, mouseCallback); // When move the mouse
   // glfwSetScrollCallback(window, scrollCallback); // When scroll the mouse

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

      projection = glm::perspective(glm::radians(camera.zoom), (float) windowWidth / windowHeight, 0.1f, 300.0f);
      view = glm::mat4(1.0f);
      view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.up);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
      model = glm::scale(model, glm::vec3(1.0f));


      model = glm::mat4(1.0f);
      for (unsigned int i = 0; i < gridWidth; ++i){
         for (unsigned int j = 0; j < gridHeight; ++j){
            model = glm::translate(model, planePos[i * gridHeight + j]);
            mainShader.use();
            mainShader.setMat4("model", model);
            mainShader.setMat4("view", view);
            mainShader.setMat4("projection", projection);

            if (track[j][i] == 0){
               mainShader.setInt("texture_diffuse1", 2);
               asphalt.draw(mainShader);
            }
            else{
               mainShader.setInt("texture_diffuse1", 3);
               grass.draw(mainShader);
            }
            
            model = glm::mat4(1.0f);
         }
      }

      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);  
      mainShader.setInt("texture_diffuse1", 4);
      car->drawCar(mainShader);

      terrainShader.use();
      terrainShader.setMat4("view", view);
      terrainShader.setMat4("projection", projection);
      terrain.render(terrainShader, camera);

      skyBox.draw(cubemapShader, projection, view);
      
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   delete car;
   glfwTerminate();

   return 0;
}
