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

void openWebsite(const std::string &url)
{
#if defined(_WIN32)
   std::string command = "start " + url;
   system(command.c_str());
#elif defined(__APPLE__)
   std::string command = "open " + url;
   system(command.c_str());
#elif defined(__linux__)
   std::string command = "xdg-open " + url;
   system(command.c_str());
#else
   std::cerr << "Unsupported platform\n";
#endif
}

ImGuiStyle &setupImGuiStyle()
{
   // Visual Studio style by MomoDeve from ImThemes
   ImGuiStyle &style = ImGui::GetStyle();

   style.Alpha = 1.0f;
   style.DisabledAlpha = 0.6000000238418579f;
   style.WindowPadding = ImVec2(8.0f, 8.0f);
   style.WindowRounding = 0.0f;
   style.WindowBorderSize = 1.0f;
   style.WindowMinSize = ImVec2(32.0f, 32.0f);
   style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
   style.WindowMenuButtonPosition = ImGuiDir_Left;
   style.ChildRounding = 0.0f;
   style.ChildBorderSize = 1.0f;
   style.PopupRounding = 0.0f;
   style.PopupBorderSize = 1.0f;
   style.FramePadding = ImVec2(4.0f, 3.0f);
   style.FrameRounding = 0.0f;
   style.FrameBorderSize = 0.0f;
   style.ItemSpacing = ImVec2(8.0f, 4.0f);
   style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
   style.CellPadding = ImVec2(4.0f, 2.0f);
   style.IndentSpacing = 21.0f;
   style.ColumnsMinSpacing = 6.0f;
   style.ScrollbarSize = 14.0f;
   style.ScrollbarRounding = 0.0f;
   style.GrabMinSize = 10.0f;
   style.GrabRounding = 0.0f;
   style.TabRounding = 0.0f;
   style.TabBorderSize = 0.0f;
   // style.TabMinWidthForCloseButton = 0.0f;
   style.ColorButtonPosition = ImGuiDir_Right;
   style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
   style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

   style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
   style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
   style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_Border] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
   style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
   style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
   style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
   style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
   style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
   style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
   style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
   style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
   style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
   style.Colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
   style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
   style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
   style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
   style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
   style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
   style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
   style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
   style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
   style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
   style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 0.0f);

   return style;
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
   ImGuiStyle &style = setupImGuiStyle();
   mainScale = 1.0f;

   style.ScaleAllSizes(mainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)

   // Set font
   style.FontScaleDpi = mainScale;
   style.FontSizeBase = 18.0f;
   ImFont *font = io.Fonts->AddFontFromFileTTF("../assets/Fonts/FiraCode-Regular.ttf");
   IM_ASSERT(font != nullptr);

   ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize renderer backends for OpenGL
   ImGui_ImplOpenGL3_Init("#version 440");

   bool drawLine = false;
   const char *drawOptions[] = {"Render full", "Render lines", "Render points"};
   int currentDrawOption = 0;
   float volume = 60.0f;
   float cameraSpeed = 50.0f;

   while (!glfwWindowShouldClose(window))
   {
      float timeValue = glfwGetTime();
      deltaTime = timeValue - lastTime;
      lastTime = timeValue;

      // New frame for ImGUI.
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();

      bool openConfirmPopup = false;
      bool enterVisitRepo = false;
      bool openConfirmQuit = false;
      bool quitButtonClicked = false;

      ImGui::NewFrame();

      ImGui::SetNextWindowSize(ImVec2(400, 500));
      ImGui::SetNextWindowPos(ImVec2(0, 0));

      if (ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
      {
         // =========================== MENU BAR =========================
         if (ImGui::BeginMenuBar())
         {
            if (ImGui::BeginMenu("Options"))
            {
               if (ImGui::MenuItem("Visit Repo"))
               {
                  openConfirmPopup = true;
               }

               if (ImGui::MenuItem("Quit", "Esc"))
               {
                  openConfirmQuit = true;
               }

               ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
         }

         // =========================== TAB BAR =========================
         ImVec2 textSize;
         static int activeTab = 0;
         if (ImGui::BeginTabBar("TabBar"))
         {
            if (ImGui::BeginTabItem("References"))
            {
               activeTab = 0;
               ImGui::NewLine();

               ImGui::SetNextItemWidth(200);
               if (ImGui::Combo("Render Option", &currentDrawOption, drawOptions, IM_ARRAYSIZE(drawOptions)))
               {
                  switch (currentDrawOption)
                  {
                  case 0:
                     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                     break;
                  case 1:
                     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                     break;
                  case 2:
                     glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                     break;
                  }
               }

               ImGui::NewLine();
               ImGui::Checkbox("Spectator Mode", &scene->camera->controllable);

               ImGui::NewLine();
               textSize = ImGui::CalcTextSize("Cursor Mode");
               ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
               ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Cursor Mode");

               std::string mode = scene->camera->isMovingFreely ? "Locked Cursor" : "Free Cursor";
               textSize = ImGui::CalcTextSize(mode.c_str());
               ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
               ImGui::Text(mode.c_str());

               ImGui::NewLine();
               if (ImGui::SliderFloat("Volume", &volume, 0.0f, 100.0f, "%.1f%"))
               {
                  scene->sound->setVolume(volume / 100.0f);
               }

               ImGui::NewLine();
               if (ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.0f, 200.0f, "%.1f%"))
               {
                  scene->camera->speedFactor = cameraSpeed;
               }

               ImGui::NewLine();

               ImGui::NewLine();
               ImVec2 windowSize = ImGui::GetWindowSize();
               float buttonWidth = 120.0f;
               float buttonHeight = 30.0f;
               float centreX = (windowSize.x - buttonWidth) * 0.5f;

               ImGui::SetCursorPosX(centreX);
               if (ImGui::Button("Respawn Car", ImVec2(buttonWidth, buttonHeight)))
               {
                  scene->car->resetPosition();
               }

               ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Instructions"))
            {
               activeTab = 1;
               {
                  std::string labelText;
                  ImVec2 textSize;
                  std::string line;
                  ImGui::BeginChild("InstructionsScrollRegion",
                                    ImVec2(0, 0),
                                    false,
                                    ImGuiWindowFlags_AlwaysVerticalScrollbar);

                  labelText = "Camera";
                  textSize = ImGui::CalcTextSize(labelText.c_str());
                  ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
                  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), labelText.c_str());
                  labelText = "Press X to enable/disable \ncursor.\n"
                              "Press W, A, S, D to \ncontrol the camera.\n"
                              "Use the mouse wheel\n to zoom in/out.\n";
                  {
                     std::istringstream iss(labelText);
                     while (std::getline(iss, line))
                     {
                        textSize = ImGui::CalcTextSize(line.c_str());
                        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
                        ImGui::Text("%s", line.c_str());
                     }
                  }

                  ImGui::Spacing();
                  ImGui::Separator();
                  ImGui::Spacing();

                  labelText = "Car";
                  textSize = ImGui::CalcTextSize(labelText.c_str());
                  ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
                  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), labelText.c_str());
                  labelText = "Press W, A, D to control\n car's movement.\n"
                              "Press S to brake.\n"
                              "Press Space to handbrake.\n"
                              "Press R to roll.\n"
                              "Press T to jump.\n"
                              "Press C to reverse.\n";

                  {
                     std::istringstream iss(labelText);
                     while (std::getline(iss, line))
                     {
                        textSize = ImGui::CalcTextSize(line.c_str());
                        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textSize.x) * 0.5f);
                        ImGui::Text("%s", line.c_str());
                     }
                  }

                  ImGui::EndChild();
               }
               ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Stats"))
            {
               std::string statsText;
               ImVec2 textSize;

               ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
               ImGui::NewLine();

               ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
         }

         // =========== CONFIRM POP UPS =============
         if (openConfirmPopup)
         {
            ImGui::OpenPopup("Confirm Visit Repo");
         }

         if (openConfirmQuit)
         {
            ImGui::OpenPopup("Confirm Quit");
         }

         // =========== POP UPS =============
         if (ImGui::BeginPopupModal("Confirm Visit Repo", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
         {
            ImGui::Text("Are you sure you want \nto visit the repository?");
            ImGui::Separator();
            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
               enterVisitRepo = true;
               openConfirmPopup = false;
               ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No", ImVec2(120, 0)))
            {
               openConfirmPopup = false;
               ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
         }

         if (ImGui::BeginPopupModal("Confirm Quit", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
         {
            ImGui::Text("Are you sure you want\nto quit?");
            ImGui::Separator();
            if (ImGui::Button("Yes", ImVec2(120, 0)))
            {
               quitButtonClicked = true;
               openConfirmQuit = false;
               ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No", ImVec2(120, 0)))
            {
               openConfirmQuit = false;
               ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
         }
      }

      if (quitButtonClicked)
      {
         glfwSetWindowShouldClose(window, true);
      }

      if (enterVisitRepo)
      {
         openWebsite("https://github.com/VietDQ1904/OpenGL-driving-simulator.git");
      }

      ImGui::End();
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
