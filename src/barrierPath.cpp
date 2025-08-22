#include "barrierPath.hpp"

void Barrier::generateVertices(Physics &simulation)
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA1, prevB1, prevA2, prevB2;

   glm::vec3 A1, B1, C1, D1; // the left side
   glm::vec3 A2, B2, C2, D2; // the right side

   glm::vec3 normal;
   glm::vec3 midPoint1, midPoint2;

   float segmentLength1, segmentLength2;
   glm::mat4 modelMatrix;
   float angle;

   int elements = 0;
   int lastIndex = 0;

   std::vector<glm::mat4> modelMatricesPartition;
   glm::vec3 pivot;

   for (int i = 0; i < generatedPath.size() - 1; ++i)
   {

      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0)
      {
         A1 = w * barrierOffset + generatedPath[i] + u * barrierHeight;
         B1 = w * barrierOffset + generatedPath[i];
         A2 = -w * barrierOffset + generatedPath[i] + u * barrierHeight;
         B2 = -w * barrierOffset + generatedPath[i];
      }
      else
      {
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

      segmentLength1 = glm::length(C1 - A1);
      segmentLength2 = glm::length(C2 - A2);
      angle = std::acos(glm::clamp(glm::dot(v, glm::vec3(0.0f, 0.0f, 1.0f)), -1.0f, 1.0f));

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength1 / modelScale, 1.0f, segmentLength1 / modelScale));
      // modelMatrices.push_back(modelMatrix);
      modelMatricesPartition.push_back(modelMatrix);

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint2);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength2 / modelScale, 1.0f, segmentLength2 / modelScale));
      // modelMatrices.push_back(modelMatrix);
      modelMatricesPartition.push_back(modelMatrix);

      // Create a rigid body.
      simulation.createRigidBody(A1, B1, C1, D1, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
      simulation.createRigidBody(A2, B2, C2, D2, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);

      if (elements++ >= partitionSize)
      {
         pivot = generatedPath[lastIndex + (i - lastIndex) / 2];
         modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatricesPartition);
         modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});

         modelMatricesPartition.clear();

         elements = 0;
         lastIndex = i;
      }
   }

   if (!modelMatricesPartition.empty())
   {
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];

      modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatricesPartition);
      modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});
   }
}

void Barrier::setUp()
{

   for (const auto &partition : modelInstances.modelMatricesList)
   {
      GLuint barrierPartitionBuffer;
      glGenBuffers(1, &barrierPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, barrierPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelInstances.insertVBO(partition.first, barrierPartitionBuffer);
   }

   for (unsigned int i = 0; i < barrierModel->meshes.size(); ++i)
   {
      GLuint meshVAO = barrierModel->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      barrierVAOs.push_back(meshVAO);

      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

      glVertexAttribDivisor(5, 1);
      glVertexAttribDivisor(6, 1);
      glVertexAttribDivisor(7, 1);
      glVertexAttribDivisor(8, 1);

      glBindVertexArray(0);
   }

   for (unsigned int i = 0; i < barrierLPModel->meshes.size(); ++i)
   {
      GLuint meshVAO = barrierLPModel->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      barrierLPVAOs.push_back(meshVAO);

      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

      glVertexAttribDivisor(5, 1);
      glVertexAttribDivisor(6, 1);
      glVertexAttribDivisor(7, 1);
      glVertexAttribDivisor(8, 1);

      glBindVertexArray(0);
   }
}

void Barrier::render(glm::mat4 view, glm::mat4 projection, Camera &camera)
{

   barrierModel->modelShader.use();
   barrierModel->modelShader.setMat4("view", view);
   barrierModel->modelShader.setMat4("projection", projection);
   barrierModel->modelShader.setVec3("viewPos", camera.cameraPos);

   barrierLPModel->modelShader.use();
   barrierLPModel->modelShader.setMat4("view", view);
   barrierLPModel->modelShader.setMat4("projection", projection);
   barrierLPModel->modelShader.setVec3("viewPos", camera.cameraPos);

   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);

   float length;
   std::vector<std::array<float, 3>> nearByPivots;
   modelInstances.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z},
                                    maxRenderDistance,
                                    nearByPivots);

   for (auto &pivot : nearByPivots)
   {
      if (camera.isInFrustum(glm::vec3(pivot[0], pivot[1], pivot[2]), 100.0f))
      {

         length = glm::distance(camera.cameraPos, glm::vec3(pivot[0], pivot[1], pivot[2]));

         if (length < renderDistance)
         {
            for (unsigned int meshIndex = 0; meshIndex < barrierModel->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(barrierModel->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelInstances.vbos[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               barrierModel->meshes[meshIndex].bindTextures(barrierModel->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(barrierModel->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelInstances.modelMatricesList[pivot].size());
            }
         }

         else
         {
            for (unsigned int meshIndex = 0; meshIndex < barrierLPModel->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(barrierLPModel->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelInstances.vbos[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               barrierLPModel->meshes[meshIndex].bindTextures(barrierLPModel->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(barrierLPModel->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelInstances.modelMatricesList[pivot].size());
            }
         }
      }
   }

   glDisable(GL_CULL_FACE);
}

void Barrier::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor)
{
   barrierModel->modelShader.use();
   barrierModel->modelShader.setVec3("light.direction", direction);
   barrierModel->modelShader.setVec3("light.color", lightColor);
   barrierLPModel->modelShader.use();
   barrierLPModel->modelShader.setVec3("light.direction", direction);
   barrierLPModel->modelShader.setVec3("light.color", lightColor);
}

void Barrier::cleanUpBuffers()
{

   modelInstances.clearBuffers();

   for (GLuint vao : barrierVAOs)
   {
      glDeleteVertexArrays(1, &vao);
   }

   for (GLuint vao : barrierLPVAOs)
   {
      glDeleteVertexArrays(1, &vao);
   }
}
