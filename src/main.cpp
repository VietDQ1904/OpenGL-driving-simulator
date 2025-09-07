#include <thread>
#include "camera.hpp"
#include "model.hpp"
#include "cubemap.hpp"
#include "car.hpp"
#include "resourceManagement.hpp"
#include "roadPath.hpp"
#include "terrainPath.hpp"
#include "barrierPath.hpp"
#include "roadSigns.hpp"
#include "grass.hpp"
#include "tree.hpp"
#include "soundTrack.hpp"

const float windowWidth = 1080.0f;
const float windowHeight = 720.0f;

// enable NVIDIA GPU rendering
extern "C"
{
   __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// // enable AMD GPU rendering
// extern "C" {
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
// }

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
   glViewport(0, 0, width, height);
}

// Check if the escape key is pressed, exit when pressed
void processInput(GLFWwindow *window, float &deltaTime)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, true);
   }
}

int main(int argc, char *argv[])
{
   glfwInit();

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_SAMPLES, 4); // Add 4 subsamples for smoother edges.

   GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Driving Simulator", nullptr, nullptr);

   if (window == nullptr)
   {
      std::cout << "OPENGL::CANNOT_INITIALIZE_GLFW\n";
      glfwTerminate();
      return -1;
   }

   glfwMakeContextCurrent(window);

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "OPENGL::CANNOT_INITIALIZE_GLAD.\n";
      return -1;
   }

   glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

   glm::mat4 model = glm::mat4(0.0f);
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection;

   glEnable(GL_DEPTH_TEST);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   float deltaTime = 0.0f;
   float lastTime = 0.0f;

   glm::vec3 lightDirection = glm::vec3(1.0f, -2.0f, -1.0f);

   ResourceManagement::loadShader("Main", "../src/Shaders/terrain.vert", "../src/Shaders/terrain.frag", nullptr);
   ResourceManagement::loadTexture("Grass", "../assets/grass.png", false, false);
   ResourceManagement::loadTexture("Asphalt", "../assets/asphalt.png", false, false);
   ResourceManagement::loadTexture("GrassSpec", "../assets/grassSpec.png", false, false);
   ResourceManagement::loadTexture("AsphaltSpec", "../assets/asphaltSpec.png", false, false);
   ResourceManagement::loadShader("Cubemap", "../src/Shaders/cubemap.vert", "../src/Shaders/cubemap.frag", nullptr);

   std::unique_ptr<Cubemap> skyBox = std::make_unique<Cubemap>();
   std::unique_ptr<Physics> simulation = std::make_unique<Physics>();
   std::unique_ptr<Sound> sound = std::make_unique<Sound>();

   sound->loadAudio("../assets/SoundTracks/carEngine.wav", "CarEngine");
   sound->loadAudio("../assets/SoundTracks/carAccelerate.wav", "CarAccelerate");
   sound->loadAudio("../assets/SoundTracks/carBrake.wav", "CarBrake");
   sound->loadAudio("../assets/SoundTracks/carHandbrake.wav", "CarHandbrake");
   sound->loadAudio("../assets/SoundTracks/carHonk.wav", "CarHonk");

   // Create main objects
   std::unique_ptr<Car> car = std::make_unique<Car>();
   std::unique_ptr<Barrier> barrier = std::make_unique<Barrier>();
   std::unique_ptr<RoadSigns> roadSigns = std::make_unique<RoadSigns>();
   std::unique_ptr<Trees> trees = std::make_unique<Trees>();
   std::unique_ptr<GrassBlades> grassBlades;
   std::unique_ptr<Terrain> terrain;
   std::unique_ptr<Road> road;

   // Create road and terrain objects in separate threads
   std::thread t1([&]
                  { road = std::make_unique<Road>(); });
   std::thread t2([&]
                  { terrain = std::make_unique<Terrain>(); });
   std::thread t3([&]
                  { grassBlades = std::make_unique<GrassBlades>(); });

   t1.join();
   t2.join();
   t3.join();

   road->addRigidBodies(*simulation);
   terrain->addRigidBodies(*simulation);
   barrier->addRigidBodies(*simulation);

   road->setUp();
   terrain->setUp();
   roadSigns->loadResources();
   roadSigns->setEnvironmentLighting(lightDirection, glm::vec3(1.0f, 1.0f, 1.0f));

   barrier->loadResources();
   barrier->setEnvironmentLighting(lightDirection, glm::vec3(1.0f, 1.0f, 1.0f));

   grassBlades->loadResources();
   grassBlades->setEnvironmentLighting(lightDirection, glm::vec3(1.0f, 1.0f, 1.0f));

   trees->loadResources();
   trees->setEnvironmentLighting(lightDirection, glm::vec3(1.0f, 1.0f, 1.0f));

   car->loadModels("../assets/Car/carBodyModel.obj", "../assets/Car/wheelModel.obj", "../assets/Car/wheelModel.obj");
   car->setUp(*simulation);
   car->loadTextures();
   car->loadShaderCarBody("CarShader", "../src/Shaders/modelTexture.vert", "../src/Shaders/modelTexture.frag", nullptr);
   car->loadShaderFrontWheels("CarShader", "../src/Shaders/modelTexture.vert", "../src/Shaders/modelTexture.frag", nullptr);
   car->loadShaderBackWheels("CarShader", "../src/Shaders/modelTexture.vert", "../src/Shaders/modelTexture.frag", nullptr);
   car->setEnvironmentLighting(lightDirection, glm::vec3(1.0f, 1.0f, 1.0f));

   Camera camera(glm::vec3(-1.0f, 1.0f, 0.0f));
   camera.setPositionToCar(*car);

   Shader mainShader = ResourceManagement::getShader("Main");
   mainShader.use();

   Shader cubemapShader = ResourceManagement::getShader("Cubemap");
   cubemapShader.use();

   glfwSetWindowUserPointer(window, &camera);
   glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y)
                            {
      if (Camera *cam = static_cast<Camera*> (glfwGetWindowUserPointer(window))){
         cam->mouseCallback(window, x, y);
      } });

   glfwSetScrollCallback(window, [](GLFWwindow *window, double scroll_x, double scroll_y)
                         {
      if (Camera *cam = static_cast<Camera*> (glfwGetWindowUserPointer(window))){
         cam->scrollCallback(window, scroll_x, scroll_y);
      } });

   float maxSecPerFrame = 1.0f / 60.0f;
   // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   sound->playAudio("CarEngine", true);

   while (!glfwWindowShouldClose(window))
   {
      float timeValue = glfwGetTime();
      deltaTime = timeValue - lastTime;
      lastTime = timeValue;

      processInput(window, deltaTime);

      // camera.updateFollowCamera(car->car);
      camera.control(window, deltaTime);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

      simulation->dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime : maxSecPerFrame), 10);

      projection = camera.getProjectionMatrix(static_cast<float>(windowWidth / windowHeight));
      view = camera.getViewMatrix();

      camera.calculateFrustrumPlanes(projection * view, static_cast<float>(windowWidth / windowHeight));

      mainShader.use();

      glActiveTexture(GL_TEXTURE0);
      ResourceManagement::getTexture("Grass").bindTexture();
      glActiveTexture(GL_TEXTURE1);
      ResourceManagement::getTexture("GrassSpec").bindTexture();
      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);
      mainShader.setInt("texture_diffuse1", 0);
      mainShader.setInt("texture_specular1", 1);
      mainShader.setVec3("viewPos", camera.cameraPos);
      mainShader.setVec3("light.direction", lightDirection);
      mainShader.setVec3("light.ambient", glm::vec3(0.6f));
      mainShader.setVec3("light.diffuse", glm::vec3(0.7f));
      mainShader.setVec3("light.specular", glm::vec3(0.6f));
      terrain->render(mainShader, camera);

      glActiveTexture(GL_TEXTURE2);
      ResourceManagement::getTexture("Asphalt").bindTexture();
      glActiveTexture(GL_TEXTURE3);
      ResourceManagement::getTexture("AsphaltSpec").bindTexture();
      mainShader.setMat4("view", view);
      mainShader.setMat4("projection", projection);
      mainShader.setInt("texture_diffuse1", 2);
      mainShader.setInt("texture_specular1", 3);
      mainShader.setVec3("viewPos", camera.cameraPos);
      mainShader.setVec3("light.direction", lightDirection);
      mainShader.setVec3("light.ambient", glm::vec3(0.6f));
      mainShader.setVec3("light.diffuse", glm::vec3(0.7f));
      mainShader.setVec3("light.specular", glm::vec3(0.6f));
      road->render(mainShader, camera);

      // car->control(window, deltaTime, *sound);
      // car->update();
      // car->render(view, projection, camera.cameraPos);

      barrier->render(view, projection, camera);
      roadSigns->render(view, projection, camera);

      grassBlades->setWindParameters(static_cast<float>(glfwGetTime()),
                                     glm::normalize(glm::vec3(1.0f, 0.5f, -0.5f)),
                                     0.05f);
      grassBlades->render(view, projection, camera);
      trees->render(view, projection, camera);

      skyBox->draw(cubemapShader, projection, view);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   ResourceManagement::clearResources();
   glfwTerminate();

   return 0;
}
