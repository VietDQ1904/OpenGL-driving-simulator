#include "camera.hpp"


void Camera::mouseCallback(GLFWwindow *window, double xpos, double ypos){
   
   if (this->firstMouse){
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
   this->pitch = std::max(std::min(this->pitch + offsetY, 90.0f), -0.0f);
   
   glm::vec3 direction;
   direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
   direction.y = sin(glm::radians(this->pitch));
   direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

   this->cameraFront = glm::normalize(direction);
   this->right = glm::normalize(glm::cross(this->cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
   this->up = glm::normalize(glm::cross(this->right, this->cameraFront));
}

void Camera::scrollCallback(GLFWwindow *window, double offsetX, double offsetY){
   this->zoom = std::max(std::min(this->zoom - (float) offsetY, 45.0f), 1.0f);
}

void Camera::updateFollowCamera(btRigidBody* car) {
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

void Camera::setPositionToCar(Car *car){
   glm::vec3 offsetBehind = car->carRot * glm::vec3(0.0f, 3.0f, 8.0f); 
   this->cameraPos = car->carPos + offsetBehind;
   this->cameraFront = glm::normalize(car->carPos - this->cameraPos);
   this->right = glm::normalize(glm::cross(this->cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
   this->up = glm::normalize(glm::cross(this->right, this->cameraFront));
}
