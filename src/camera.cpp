#include "camera.hpp"

void Camera::mouseCallback(GLFWwindow *window, double xpos, double ypos)
{

   if (this->firstMouse)
   {
      this->lastX = xpos;
      this->lastY = ypos;
      this->firstMouse = false;
   }

   // Get the distance the mouse moved
   float offsetX = xpos - this->lastX;
   float offsetY = this->lastY - ypos; // Reversed because y ranges from bottom to top

   this->lastX = xpos;
   this->lastY = ypos;

   const float sensitivity = 0.1f;
   offsetX *= sensitivity;
   offsetY *= sensitivity;

   this->yaw += offsetX;
   // Add constraints to pitch (so that you cannot rotate up 360 degrees)
   this->pitch = std::max(std::min(this->pitch + offsetY, 90.0f), -90.0f);

   glm::vec3 direction;
   direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
   direction.y = sin(glm::radians(this->pitch));
   direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

   this->cameraFront = glm::normalize(direction);
   this->right = glm::normalize(glm::cross(this->cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
   this->up = glm::normalize(glm::cross(this->right, this->cameraFront));
}

void Camera::control(GLFWwindow *window, float deltaTime)
{
   const float cameraSpeed = 10.0f * deltaTime;
   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
   {
      cameraPos += cameraFront * cameraSpeed;
   }
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
   {
      cameraPos -= cameraFront * cameraSpeed;
   }
   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
   {
      cameraPos -= right * cameraSpeed;
   }
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
   {
      cameraPos += right * cameraSpeed;
   }
}

void Camera::scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
   this->zoom = std::max(std::min(this->zoom - (float)offsetY, 45.0f), 1.0f);
}

void Camera::updateFollowCamera(btRigidBody *car)
{
   btTransform carTransform;
   car->getMotionState()->getWorldTransform(carTransform);

   btVector3 carPos = carTransform.getOrigin();
   btQuaternion carRot = carTransform.getRotation();

   glm::vec3 carPosition(carPos.getX(), carPos.getY(), carPos.getZ());
   glm::quat carRotation(carRot.getW(), carRot.getX(), carRot.getY(), carRot.getZ());

   glm::vec3 offset;
   float camDistance = 10.0f;

   offset.x = camDistance * cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
   offset.y = camDistance * sin(glm::radians(this->pitch));
   offset.z = camDistance * cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));

   glm::vec3 camWorldOffset = carRotation * offset;

   this->cameraPos = carPosition + camWorldOffset;
   // Prevents the camera from seeing from the underground.
   this->cameraPos.y = std::max(carPosition.y + camWorldOffset.y, carPosition.y);

   this->cameraFront = glm::normalize(carPosition - this->cameraPos);
   this->right = glm::normalize(glm::cross(this->cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
   this->up = glm::normalize(glm::cross(this->right, this->cameraFront));
}

void Camera::setPositionToCar(Car &car)
{
   glm::vec3 offsetBehind = car.carRot * glm::vec3(0.0f, 3.0f, 8.0f);
   this->cameraPos = car.carPos + offsetBehind;
   this->cameraFront = glm::normalize(car.carPos - this->cameraPos);
   this->right = glm::normalize(glm::cross(this->cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
   this->up = glm::normalize(glm::cross(this->right, this->cameraFront));
}

glm::mat4 Camera::getViewMatrix() const
{
   return glm::lookAt(cameraPos, cameraPos + cameraFront, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const
{
   return glm::perspective(glm::radians(zoom), aspect, nearPlane, farPlane);
}

void Camera::calculateFrustrumPlanes(const glm::mat4 &projectionView, float aspect)
{
   glm::vec4 rowX = glm::vec4(projectionView[0][0], projectionView[1][0], projectionView[2][0], projectionView[3][0]);
   glm::vec4 rowY = glm::vec4(projectionView[0][1], projectionView[1][1], projectionView[2][1], projectionView[3][1]);
   glm::vec4 rowZ = glm::vec4(projectionView[0][2], projectionView[1][2], projectionView[2][2], projectionView[3][2]);
   glm::vec4 rowW = glm::vec4(projectionView[0][3], projectionView[1][3], projectionView[2][3], projectionView[3][3]);

   // Left
   this->frustum[0].normal = glm::vec3(rowW + rowX);
   this->frustum[0].distance = (rowW + rowX).w;

   // Right
   this->frustum[1].normal = glm::vec3(rowW - rowX);
   this->frustum[1].distance = (rowW - rowX).w;

   // Bottom
   this->frustum[2].normal = glm::vec3(rowW + rowY);
   this->frustum[2].distance = (rowW + rowY).w;

   // Top
   this->frustum[3].normal = glm::vec3(rowW - rowY);
   this->frustum[3].distance = (rowW - rowY).w;

   // Near
   this->frustum[4].normal = glm::vec3(rowW + rowZ);
   this->frustum[4].distance = (rowW + rowZ).w;

   // Far
   this->frustum[5].normal = glm::vec3(rowW - rowZ);
   this->frustum[5].distance = (rowW - rowZ).w;

   for (int i = 0; i < 6; ++i)
   {
      float length = glm::length(frustum[i].normal);
      frustum[i].normal /= length;
      frustum[i].distance /= length;
   }
}

bool Camera::isInFrustum(const glm::vec3 &point, float range) const
{
   for (int i = 0; i < 6; ++i)
   {
      float dist = glm::dot(frustum[i].normal, point) + frustum[i].distance;
      if (dist < -range)
      {
         return false;
      }
   }
   return true;
}
