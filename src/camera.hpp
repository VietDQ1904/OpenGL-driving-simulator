#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "physics.hpp"
#include "car.hpp"

#ifndef CAMERA
#define CAMERA

struct FrustumPlane
{
   glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
   float distance = 0.0f;
};

class Camera
{
public:
   float lastX = 400.0f, lastY = 300.0f;
   float yaw = -90.0f, pitch = 8.0f;
   float zoom = 45.0f;
   glm::vec3 cameraPos;
   glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
   bool firstMouse = true;

   float nearPlane = 0.1f;
   float farPlane = 300.0f;

   Camera(glm::vec3 cameraPos) : cameraPos(cameraPos) {}
   void mouseCallback(GLFWwindow *window, double xpos, double ypos);
   void scrollCallback(GLFWwindow *window, double offsetX, double offsetY);
   void updateFollowCamera(btRigidBody *car);
   void setPositionToCar(Car &car);
   glm::mat4 getViewMatrix() const;
   glm::mat4 getProjectionMatrix(float aspect) const;
   void calculateFrustrumPlanes(const glm::mat4 &projectionView, float aspect);
   bool isInFrustum(const glm::vec3 &center, float radius) const;

private:
   std::array<FrustumPlane, 6> frustum;
};

#endif