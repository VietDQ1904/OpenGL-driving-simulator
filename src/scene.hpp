#include <thread>
#include "camera.hpp"
#include "cubemap.hpp"
#include "car.hpp"
#include "roadPath.hpp"
#include "terrainPath.hpp"
#include "barrierPath.hpp"
#include "roadSigns.hpp"
#include "grass.hpp"
#include "tree.hpp"
#include "soundTrack.hpp"

class Scene
{
public:
   glm::vec3 lightDirection = glm::vec3(1.0f, -2.0f, -1.0f);
   const float frameRate = 60.0f;

   // Main Objects
   std::unique_ptr<Car> car = std::make_unique<Car>();
   std::unique_ptr<Barrier> barrier = std::make_unique<Barrier>();
   std::unique_ptr<RoadSigns> roadSigns = std::make_unique<RoadSigns>();
   std::unique_ptr<Trees> trees = std::make_unique<Trees>();
   std::unique_ptr<GrassBlades> grassBlades;
   std::unique_ptr<Terrain> terrain;
   std::unique_ptr<Road> road;

   std::unique_ptr<Cubemap> skyBox = std::make_unique<Cubemap>();
   std::unique_ptr<Physics> simulation = std::make_unique<Physics>();
   std::unique_ptr<Sound> sound = std::make_unique<Sound>();
   std::unique_ptr<Camera> camera = std::make_unique<Camera>(glm::vec3(-1.0f, 1.0f, 0.0f));

   Shader mainShader;
   Shader cubemapShader;

   glm::mat4 model = glm::mat4(0.0f);
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection;

   float maxSecPerFrame = 1.0f / frameRate;

   bool isFollowingCar = false;

   void loadScene(GLFWwindow *window);
   void updateScene(float deltaTime, const float windowWidth, const float windowHeight);
   void renderScene(GLFWwindow *window, float deltaTime);
};

inline void Scene::loadScene(GLFWwindow *window)
{
   ResourceManagement::loadShader("Main", "../src/Shaders/terrain.vert", "../src/Shaders/terrain.frag", nullptr);
   ResourceManagement::loadTexture("Grass", "../assets/grass.png", false, false);
   ResourceManagement::loadTexture("Asphalt", "../assets/asphalt.png", false, false);
   ResourceManagement::loadTexture("GrassSpec", "../assets/grassSpec.png", false, false);
   ResourceManagement::loadTexture("AsphaltSpec", "../assets/asphaltSpec.png", false, false);
   ResourceManagement::loadShader("Cubemap", "../src/Shaders/cubemap.vert", "../src/Shaders/cubemap.frag", nullptr);

   sound->loadAudio("../assets/SoundTracks/carEngine.wav", "CarEngine");
   sound->loadAudio("../assets/SoundTracks/carAccelerate.wav", "CarAccelerate");
   sound->loadAudio("../assets/SoundTracks/carBrake.wav", "CarBrake");
   sound->loadAudio("../assets/SoundTracks/carHandbrake.wav", "CarHandbrake");
   sound->loadAudio("../assets/SoundTracks/carHonk.wav", "CarHonk");
   sound->setVolume(0.6f);

   // Create main objects

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

   camera->setPositionToCar(*car);

   mainShader = ResourceManagement::getShader("Main");
   mainShader.use();

   cubemapShader = ResourceManagement::getShader("Cubemap");
   cubemapShader.use();

   sound->playAudio("CarEngine", true);

   glfwSetWindowUserPointer(window, camera.get());
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
}

inline void Scene::updateScene(float deltaTime, const float windowWidth, const float windowHeight)
{
   // if (isFollowingCar)
   // {
   //    camera->isMovingFreely = true;
   // }
   // else
   // {
   //    camera->isMovingFreely = false;
   // }
   camera->isMovingFreely = isFollowingCar;
   if (!camera->controllable)
   {
      camera->updateFollowCamera(car->car);
   }

   simulation->dynamicsWorld->stepSimulation((deltaTime < maxSecPerFrame ? deltaTime : maxSecPerFrame), 10);

   this->projection = camera->getProjectionMatrix(static_cast<float>(windowWidth / windowHeight));
   this->view = camera->getViewMatrix();

   camera->calculateFrustrumPlanes(this->projection * this->view, static_cast<float>(windowWidth / windowHeight));
}

inline void Scene::renderScene(GLFWwindow *window, float deltaTime)
{
   if (camera->controllable)
   {
      camera->control(window, deltaTime);
   }

   mainShader.use();
   glActiveTexture(GL_TEXTURE0);
   ResourceManagement::getTexture("Grass").bindTexture();
   glActiveTexture(GL_TEXTURE1);
   ResourceManagement::getTexture("GrassSpec").bindTexture();
   mainShader.setMat4("view", view);
   mainShader.setMat4("projection", projection);
   mainShader.setInt("texture_diffuse1", 0);
   mainShader.setInt("texture_specular1", 1);
   mainShader.setVec3("viewPos", camera->cameraPos);
   mainShader.setVec3("light.direction", lightDirection);
   mainShader.setVec3("light.ambient", glm::vec3(0.5f));
   mainShader.setVec3("light.diffuse", glm::vec3(0.7f));
   mainShader.setVec3("light.specular", glm::vec3(0.6f));
   terrain->render(mainShader, *camera);

   glActiveTexture(GL_TEXTURE2);
   ResourceManagement::getTexture("Asphalt").bindTexture();
   glActiveTexture(GL_TEXTURE3);
   ResourceManagement::getTexture("AsphaltSpec").bindTexture();
   mainShader.setMat4("view", view);
   mainShader.setMat4("projection", projection);
   mainShader.setInt("texture_diffuse1", 2);
   mainShader.setInt("texture_specular1", 3);
   mainShader.setVec3("viewPos", camera->cameraPos);
   mainShader.setVec3("light.direction", lightDirection);
   mainShader.setVec3("light.ambient", glm::vec3(0.5f));
   mainShader.setVec3("light.diffuse", glm::vec3(0.7f));
   mainShader.setVec3("light.specular", glm::vec3(0.6f));
   road->render(mainShader, *camera);

   if (!camera->controllable)
   {
      car->control(window, deltaTime, *sound);
   }
   car->update();
   car->render(view, projection, camera->cameraPos);

   barrier->render(view, projection, *camera);
   roadSigns->render(view, projection, *camera);

   grassBlades->setWindParameters(static_cast<float>(glfwGetTime()),
                                  glm::normalize(glm::vec3(1.0f, 0.5f, -0.5f)),
                                  0.1f);

   grassBlades->render(view, projection, *camera);
   trees->render(view, projection, *camera);

   skyBox->draw(cubemapShader, projection, view);
}
