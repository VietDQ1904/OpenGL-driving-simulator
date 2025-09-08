#include <thread>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "scene.hpp"

float mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
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
void processInput(GLFWwindow *window, float &deltaTime, Scene *scene)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, true);
   }

   static bool xKeyPressed = false;
   if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !xKeyPressed)
   {
      scene->isFollowingCar = !scene->isFollowingCar;
      xKeyPressed = true;

      if (scene->isFollowingCar)
      {
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      else
      {
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
   }

   if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
   {
      xKeyPressed = false;
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

   glEnable(GL_DEPTH_TEST);
   // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   float deltaTime = 0.0f;
   float lastTime = 0.0f;
   std::unique_ptr<Scene> scene = std::make_unique<Scene>();
   scene->loadScene(window);

   glfwSetWindowUserPointer(window, scene->camera.get());
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

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   (void)io;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

   ImGui::StyleColorsDark(); // Setup ImGui style
   ImGuiStyle &style = ImGui::GetStyle();

   if (mainScale < 1.0f)
   {
      mainScale = 1.0f;
   }

   style.ScaleAllSizes(mainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)

   // Set font
   style.FontScaleDpi = mainScale;
   style.FontSizeBase = 18.0f;
   ImFont *font = io.Fonts->AddFontFromFileTTF("../assets/Fonts/FiraCode-Regular.ttf");
   IM_ASSERT(font != nullptr);

   ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize renderer backends for OpenGL
   ImGui_ImplOpenGL3_Init("#version 440");
   bool drawLine = false;

   while (!glfwWindowShouldClose(window))
   {
      float timeValue = glfwGetTime();
      deltaTime = timeValue - lastTime;
      lastTime = timeValue;

      // New frame for ImGUI.
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();

      ImGui::NewFrame();
      {
         ImGui::SetNextWindowPos(ImVec2(0, 0));
         ImGui::SetNextWindowSize(ImVec2(250, windowHeight));
         ImGui::Begin("Debug Window", nullptr,
                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
         ImGui::Text("FPS: %.1f", io.Framerate);

         ImGui::NewLine();
         if (ImGui::Checkbox("Draw Lines", &drawLine))
         {
            if (drawLine)
            {
               glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
               glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
         }

         ImGui::NewLine();
         ImGui::Checkbox("Spectator Mode", &scene->camera->controllable);

         ImGui::NewLine();
         ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Cursor");

         std::string mode = scene->camera->isMovingFreely ? "Locked Cursor" : "Free Cursor";
         ImGui::Text(mode.c_str());

         ImGui::NewLine();

         if (ImGui::Button("Respawn Car"))
         {
            scene->car->resetPosition();
         }

         ImGui::NewLine();
         ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Camera");
         ImGui::Text("Press W,A,S,D to \nnavigate.");
         ImGui::Text("Press X to \nenable/disable cursor");
         ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Car");
         ImGui::Text("Press W,A,D to \ncontrol the car.\n");
         ImGui::Text("Press S to brake.\n");
         ImGui::Text("Press Space to \nhandbrake.\n");
         ImGui::Text("Press R to roll.\n");
         ImGui::Text("Press T to jump.\n");
         ImGui::Text("Press C to \ngo backward.\n");

         ImGui::End();
      }

      ImGui::Render(); // End frame

      processInput(window, deltaTime, scene.get());

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

      scene->updateScene(deltaTime, windowWidth, windowHeight);
      scene->renderScene(window, deltaTime);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Draw data.

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   ResourceManagement::clearResources();
   glfwTerminate();

   return 0;
}
