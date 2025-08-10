#include "barrierPath.hpp"

void Barrier::generateVertices(Physics &simulation){
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA1, prevB1, prevA2, prevB2;

   glm::vec3 A1, B1, C1, D1; // the left side 
   glm::vec3 A2, B2, C2, D2; // the right side

   glm::vec3 normal;
   glm::vec3 midPoint1, midPoint2;

   float segmentLength;
   glm::mat4 modelMatrix;
   float angle;

   for (int i = 0; i < generatedPath.size() - 1; ++i){

      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0){
         A1 = w * barrierOffset + generatedPath[i] + u * barrierHeight;
         B1 = w * barrierOffset + generatedPath[i];
         A2 = -w * barrierOffset + generatedPath[i] + u * barrierHeight;
         B2 = -w * barrierOffset + generatedPath[i];
      }
      else{
         A1 = prevA1;
         B1 = prevB1;
         A2 = prevA2;
         B2 = prevB2;
      }

      C1 = w * barrierOffset + generatedPath[i + 1] + u * barrierHeight;
      D1 = w * barrierOffset + generatedPath[i + 1];
      C2 = -w * barrierOffset + generatedPath[i + 1] + u * barrierHeight;
      D2 = -w * barrierOffset + generatedPath[i + 1];

      // Save point C1, D1 of the current iteration.
      prevA1 = C1;
      prevB1 = D1;
      prevA2 = C2;
      prevB2 = D2;

      midPoint1 = (B1 + D1) / 2.0f;
      midPoint2 = (B2 + D2) / 2.0f;

      segmentLength = glm::length(generatedPath[i + 1] - generatedPath[i]);
      angle = std::acos(glm::clamp(glm::dot(v, glm::vec3(0.0f, 0.0f, 1.0f)), -1.0f, 1.0f));
      
      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength / 3.0f, 1.0f, segmentLength / 1.75f));
      modelMatrices.push_back(modelMatrix);

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint2);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength / 3.0f, 1.0f, segmentLength / 1.75f));
      modelMatrices.push_back(modelMatrix);

      // Create a rigid body.
      simulation.createRigidBody(A1, B1, C1, D1, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
      simulation.createRigidBody(A2, B2, C2, D2, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
   }

}

void Barrier::setUp(){

   glGenBuffers(1, &barrierBuffer);
   glBindBuffer(GL_ARRAY_BUFFER, barrierBuffer);
   glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

   for (unsigned int i = 0; i < barrierModel->meshes.size(); ++i){
      GLuint meshVAO = barrierModel->meshes[i].vao;
      glBindVertexArray(meshVAO);

      size_t matrixSegment = sizeof(glm::vec4);

      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *) 0);
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *) (matrixSegment));
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *) (2 * matrixSegment));
      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *) (3 * matrixSegment));

      glVertexAttribDivisor(5, 1);
      glVertexAttribDivisor(6, 1);
      glVertexAttribDivisor(7, 1);
      glVertexAttribDivisor(8, 1);
      
      glBindVertexArray(0);
   }


}

void Barrier::render(glm::mat4 view, glm::mat4 projection, glm::vec3 viewPos){

   barrierModel->modelShader.use();
   barrierModel->modelShader.setMat4("view", view);
   barrierModel->modelShader.setMat4("projection", projection);
   barrierModel->modelShader.setVec3("viewPos", viewPos);

   for (unsigned int i = 0; i < barrierModel->meshes.size(); ++i){
      barrierModel->meshes[i].bindTextures(barrierModel->modelShader);
      glBindVertexArray(barrierModel->meshes[i].vao);
      // glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(barrierModel->meshes[i].indices.size()), 
      // GL_UNSIGNED_INT, 0, modelMatrices.size());
      glBindVertexArray(0);
   }

}

void Barrier::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor){
   barrierModel->modelShader.use();
   barrierModel->modelShader.setVec3("light.direction", direction);
   barrierModel->modelShader.setVec3("light.color", lightColor);  
}

void Barrier::cleanUpBuffers(){
   glDeleteBuffers(1, &barrierBuffer);
}
