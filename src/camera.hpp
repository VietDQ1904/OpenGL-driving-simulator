#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "physics.hpp"
#include "car.hpp"

#ifndef CAMERA_SOURCE
#define CAMERA_SOURCE

class Camera{
   public:
      float lastX = 400.0f, lastY = 300.0f;
      float yaw = -90.0f, pitch = 8.0f;
      float zoom = 45.0f;
      glm::vec3 cameraPos; 
      glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
      glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
      bool firstMouse = true;

      Camera(glm::vec3 cameraPos): cameraPos(cameraPos){}
      void mouseCallback(GLFWwindow *window, double xpos, double ypos);
      void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
      void updateFollowCamera(btRigidBody* car);
      void setPositionToCar(Car *car);
      glm::mat4 getViewMatrix() const;
      glm::mat4 getProjectionMatrix(float aspect) const;
};

#endif