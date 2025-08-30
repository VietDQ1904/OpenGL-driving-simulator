#include "barrierPath.hpp"

glm::vec3 getCrossVector(glm::vec3 v1, glm::vec3 v2)
{
   glm::vec3 cross = glm::cross(glm::normalize(v1), glm::normalize(v2));
   float length = glm::length(cross);
   return (length > 1e-5f) ? glm::normalize(cross) : glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 getNormalVector(std::vector<glm::vec3> &points, glm::vec3 u, size_t index)
{
   if (index == 0)
   {
      glm::vec3 edge = points[1] - points[0];
      return getCrossVector(edge, u);
   }
   else if (index == points.size() - 1)
   {
      glm::vec3 edge = points[index] - points[index - 1];
      return getCrossVector(edge, u);
   }
   else
   {
      glm::vec3 prevEdge = points[index] - points[index - 1];
      glm::vec3 nextEdge = points[index + 1] - points[index];

      glm::vec3 prevNormal = getCrossVector(prevEdge, u);
      glm::vec3 nextNormal = getCrossVector(nextEdge, u);

      glm::vec3 avgNormal = prevNormal + nextNormal;
      return (glm::length(avgNormal) > 1e-5f) ? glm::normalize(avgNormal) : prevNormal;
   }
}

void Barrier::offsetPaths()
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 normal;

   if (points.size() < 2)
   {
      return;
   }

   leftPoints.reserve(points.size());
   rightPoints.reserve(points.size());

   for (size_t i = 0; i < points.size(); ++i)
   {
      normal = getNormalVector(points, u, i);
      glm::vec3 parallelLeftPoint = points[i] - normal * barrierOffset * 0.85f;
      glm::vec3 parallelRightPoint = points[i] + normal * barrierOffset * 1.35f;
      leftPoints.push_back(parallelLeftPoint);
      rightPoints.push_back(parallelRightPoint);
   }
}

void Barrier::generateVertices(Physics &simulation)
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA, prevB;
   glm::vec3 A, B, C, D;

   glm::vec3 normal;
   glm::vec3 midPoint1, midPoint2;

   float segmentLength;
   glm::mat4 modelMatrix;
   float angle;

   int elements = 0;
   int lastIndex = 0;

   std::vector<glm::mat4> modelMatricesPartition;
   glm::vec3 pivot;

   for (size_t i = 0; i < generatedLeftPath.size() - 1; ++i)
   {
      v = glm::normalize(generatedLeftPath[i + 1] - generatedLeftPath[i]);
      w = glm::normalize(glm::cross(u, v));
      if (i == 0)
      {
         A = w * barrierOffset + generatedLeftPath[i] + u * barrierHeight;
         B = w * barrierOffset + generatedLeftPath[i];
      }
      else
      {
         A = prevA;
         B = prevB;
      }

      C = w * barrierOffset + generatedLeftPath[i + 1] + u * barrierHeight;
      D = w * barrierOffset + generatedLeftPath[i + 1];

      // Save point C, D of the current iteration.
      prevA = C;
      prevB = D;

      midPoint1 = (B + D) / 2.0f;

      segmentLength = glm::length(C - A);
      angle = std::acos(glm::clamp(glm::dot(v, glm::vec3(0.0f, 0.0f, 1.0f)), -1.0f, 1.0f));

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength / modelScale, 1.0f, segmentLength / modelScale));
      modelMatricesPartition.push_back(modelMatrix);

      simulation.createRigidBody(A, B, C, D, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);

      if (elements++ >= partitionSize)
      {
         pivot = generatedLeftPath[lastIndex + (i - lastIndex) / 2];
         modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatricesPartition);
         modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});

         modelMatricesPartition.clear();

         elements = 0;
         lastIndex = i;
      }
   }

   if (!modelMatricesPartition.empty())
   {
      pivot = generatedLeftPath[lastIndex + (generatedLeftPath.size() - 1 - lastIndex) / 2];
      modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatricesPartition);
      modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});
   }

   elements = 0;
   lastIndex = 0;

   for (size_t i = 0; i < generatedRightPath.size() - 1; ++i)
   {
      v = glm::normalize(generatedRightPath[i + 1] - generatedRightPath[i]);
      w = glm::normalize(glm::cross(u, v));
      if (i == 0)
      {
         A = w * barrierOffset + generatedRightPath[i] + u * barrierHeight;
         B = w * barrierOffset + generatedRightPath[i];
      }
      else
      {
         A = prevA;
         B = prevB;
      }

      C = w * barrierOffset + generatedRightPath[i + 1] + u * barrierHeight;
      D = w * barrierOffset + generatedRightPath[i + 1];

      // Save point C, D of the current iteration.
      prevA = C;
      prevB = D;

      midPoint1 = (B + D) / 2.0f;

      segmentLength = glm::length(C - A);

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, midPoint1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, 0.0f, 1.0f), v));
      modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(segmentLength / modelScale, 1.0f, segmentLength / modelScale));
      modelMatricesPartition.push_back(modelMatrix);

      simulation.createRigidBody(A, B, C, D, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);

      if (elements++ >= partitionSize)
      {
         pivot = generatedRightPath[lastIndex + (i - lastIndex) / 2];
         modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatricesPartition);
         modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});

         modelMatricesPartition.clear();

         elements = 0;
         lastIndex = i;
      }
   }

   if (!modelMatricesPartition.empty())
   {
      pivot = generatedRightPath[lastIndex + (generatedRightPath.size() - 1 - lastIndex) / 2];
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
