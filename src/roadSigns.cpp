#include "roadSigns.hpp"

float getOrientedAngle(glm::vec3 v1, glm::vec3 v2, glm::vec3 u)
{
   v1 = glm::normalize(v1);
   v2 = glm::normalize(v2);
   u = glm::normalize(u);

   float dot = glm::dot(v1, v2);
   glm::vec3 cross = glm::cross(v1, v2);
   float angle = std::atan2f(glm::dot(u, cross), dot);

   return angle;
}

void RoadSigns::generateModels()
{
   glm::vec3 current;
   glm::vec3 next;
   glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 mid;
   glm::vec3 w;

   glm::vec3 dir_1;
   glm::vec3 dir_2;

   glm::vec3 pos_1;
   glm::vec3 pos_2;

   glm::mat4 modelMatrix;
   std::vector<glm::mat4> modelMatrices;

   int curveDirection = STRAIGHT;
   float firstCurve;
   float angle;
   int elements = 0;
   int lastIndex = 0;

   std::vector<int> directions(points.size(), STRAIGHT);
   std::vector<int> directions2(points.size(), STRAIGHT);
   std::vector<std::array<float, 3>> pivots(points.size(), {0.0f, 0.0f, 0.0f});
   std::vector<std::array<float, 3>> pivots2(points.size(), {0.0f, 0.0f, 0.0f});

   for (int i = 0; i < points.size() - 2; ++i)
   {
      current = points[i + 1] - points[i];
      next = points[i + 2] - points[i + 1];

      angle = getOrientedAngle(current, next, up);
      angle = glm::degrees(angle);

      if (std::abs(angle) <= 20.0f)
      {
         continue;
      }

      if (angle > 0)
      {
         directions[i] = LEFT_CURVE;
      }
      else
      {
         directions[i] = RIGHT_CURVE;
      }

      if (i > 0)
      {
         if (directions[i - 1] == LEFT_CURVE && directions[i] == RIGHT_CURVE)
         {
            directions[i] = STRAIGHT;
            directions[i - 1] = LEFT_RIGHT_CURVE;
         }
         else if (directions[i - 1] == RIGHT_CURVE && directions[i] == LEFT_CURVE)
         {
            directions[i] = STRAIGHT;
            directions[i - 1] = RIGHT_LEFT_CURVE;
         }
         else if (directions[i - 1] == LEFT_CURVE && directions[i] == LEFT_CURVE)
         {
            if (std::abs(angle) >= 60.0f)
            {
               directions[i] = STRAIGHT;
               directions[i - 1] = SHARP_LEFT_CURVE;
            }
            else
            {
               directions[i] = STRAIGHT;
               directions[i - 1] = LEFT_CURVE;
            }
         }
         else if (directions[i - 1] == RIGHT_CURVE && directions[i] == RIGHT_CURVE)
         {
            if (std::abs(angle) >= 60.0f)
            {
               directions[i] = STRAIGHT;
               directions[i - 1] = SHARP_RIGHT_CURVE;
            }
            else
            {
               directions[i] = STRAIGHT;
               directions[i - 1] = RIGHT_CURVE;
            }
         }
      }

      if (directions[i] == STRAIGHT)
      {
         continue;
      }

      int mid_indx = static_cast<int>((i * (points.size() - 3) + (i + 1) * (points.size() - 3)) * 0.5f);
      mid = generatedPath[mid_indx];

      dir_1 = glm::normalize(glm::cross(glm::normalize(generatedPath[mid_indx + 1] - generatedPath[mid_indx]), up));
      pos_1 = dir_1 * (pathWidth / 2.0f + offset) + mid;

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, pos_1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), dir_1));
      modelMatrices.push_back(modelMatrix);

      std::array<float, 3> pivot = {pos_1.x, pos_1.y, pos_1.z};
      pivots[i] = pivot;

      modelInstances.insertModelMatrices(pivot, modelMatrices);
      modelInstances.insertGridMap(pivot);
      modelMatrices.clear();
   }

   for (int i = points.size() - 1; i >= 2; --i)
   {
      current = points[i] - points[i - 1];
      next = points[i - 1] - points[i - 2];

      angle = getOrientedAngle(current, next, up);
      angle = glm::degrees(angle);

      if (std::abs(angle) <= 20.0f)
      {
         continue;
      }

      if (angle > 0)
      {
         directions2[i] = LEFT_CURVE;
      }
      else
      {
         directions2[i] = RIGHT_CURVE;
      }

      if (i > 0)
      {
         if (directions2[i + 1] == LEFT_CURVE && directions2[i] == RIGHT_CURVE)
         {
            directions2[i] = STRAIGHT;
            directions2[i + 1] = LEFT_RIGHT_CURVE;
         }
         else if (directions2[i + 1] == RIGHT_CURVE && directions2[i] == LEFT_CURVE)
         {
            directions2[i] = STRAIGHT;
            directions2[i + 1] = RIGHT_LEFT_CURVE;
         }
         else if (directions2[i + 1] == LEFT_CURVE && directions2[i] == LEFT_CURVE)
         {
            if (std::abs(angle) >= 60.0f)
            {
               directions2[i] = STRAIGHT;
               directions2[i + 1] = SHARP_LEFT_CURVE;
            }
            else
            {
               directions2[i] = STRAIGHT;
               directions2[i + 1] = LEFT_CURVE;
            }
         }
         else if (directions2[i + 1] == RIGHT_CURVE && directions2[i] == RIGHT_CURVE)
         {
            if (std::abs(angle) >= 60.0f)
            {
               directions2[i] = STRAIGHT;
               directions2[i + 1] = SHARP_RIGHT_CURVE;
            }
            else
            {
               directions2[i] = STRAIGHT;
               directions2[i + 1] = RIGHT_CURVE;
            }
         }
      }

      if (directions2[i] == STRAIGHT)
      {
         continue;
      }

      int mid_indx = static_cast<int>((i * (points.size() - 3) + (i + 1) * (points.size() - 3)) * 0.5f);
      mid = generatedPath[mid_indx];

      dir_1 = glm::normalize(glm::cross(glm::normalize(generatedPath[mid_indx] - generatedPath[mid_indx + 1]), up));
      pos_1 = dir_1 * (pathWidth / 2.0f + offset) + mid;

      modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, pos_1);
      modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), dir_1));
      modelMatrices.push_back(modelMatrix);

      std::array<float, 3> pivot = {pos_1.x, pos_1.y, pos_1.z};
      pivots2[i] = pivot;

      modelInstances.insertModelMatrices(pivot, modelMatrices);
      modelInstances.insertGridMap(pivot);
      modelMatrices.clear();
   }

   for (int i = 0; i < directions.size(); ++i)
   {
      std::array<float, 3> pivot = pivots[i];
      if (pivot[0] != 0.0f && pivot[1] != 0.0f && pivot[2] != 0.0f)
      {
         directionsMap[pivot] = directions[i];
      }
   }

   for (int i = 0; i < directions2.size(); ++i)
   {
      std::array<float, 3> pivot = pivots2[i];
      if (pivot[0] != 0.0f && pivot[1] != 0.0f && pivot[2] != 0.0f)
      {
         directionsMap[pivot] = directions2[i];
      }
   }
}

void RoadSigns::setUp()
{
   for (const auto &partition : modelInstances.modelMatricesList)
   {
      GLuint barrierPartitionBuffer;
      glGenBuffers(1, &barrierPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, barrierPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelInstances.insertVBO(partition.first, barrierPartitionBuffer);
   }

   for (unsigned int i = 0; i < signModel->meshes.size(); ++i)
   {
      GLuint meshVAO = signModel->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      signVAOs.push_back(meshVAO);

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

   for (unsigned int i = 0; i < signModelLP->meshes.size(); ++i)
   {
      GLuint meshVAO = signModelLP->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      signLPVAOs.push_back(meshVAO);

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

void RoadSigns::render(glm::mat4 view, glm::mat4 projection, Camera &camera)
{
   signModel->modelShader.use();
   signModel->modelShader.setMat4("view", view);
   signModel->modelShader.setMat4("projection", projection);
   signModel->modelShader.setVec3("viewPos", camera.cameraPos);

   signModelLP->modelShader.use();
   signModelLP->modelShader.setMat4("view", view);
   signModelLP->modelShader.setMat4("projection", projection);
   signModelLP->modelShader.setVec3("viewPos", camera.cameraPos);

   float length;
   std::vector<std::array<float, 3>> nearByPivots;
   modelInstances.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z},
                                    maxRenderDistance,
                                    nearByPivots);

   for (auto &pivot : nearByPivots)
   {
      length = glm::distance(camera.cameraPos, glm::vec3(pivot[0], pivot[1], pivot[2]));
      if (length < renderDistance)
      {
         for (unsigned int meshIndex = 0; meshIndex < signModel->meshes.size(); ++meshIndex)
         {
            glBindVertexArray(signModel->meshes[meshIndex].vao);
            glBindBuffer(GL_ARRAY_BUFFER, modelInstances.vbos[pivot]);

            size_t matrixSegment = sizeof(glm::vec4);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

            int indx = directionsMap[pivot];
            signModel->setDiffuseTexture(0, diffuseTextures[indx]);

            signModel->meshes[meshIndex].bindTextures(signModel->modelShader);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<unsigned int>(signModel->meshes[meshIndex].indices.size()),
                                    GL_UNSIGNED_INT,
                                    0,
                                    modelInstances.modelMatricesList[pivot].size());
         }
      }
      else
      {
         for (unsigned int meshIndex = 0; meshIndex < signModelLP->meshes.size(); ++meshIndex)
         {
            glBindVertexArray(signModelLP->meshes[meshIndex].vao);
            glBindBuffer(GL_ARRAY_BUFFER, modelInstances.vbos[pivot]);

            int indx = directionsMap[pivot];
            signModel->setDiffuseTexture(0, diffuseTextures[indx]);

            size_t matrixSegment = sizeof(glm::vec4);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

            signModelLP->meshes[meshIndex].bindTextures(signModelLP->modelShader);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<unsigned int>(signModelLP->meshes[meshIndex].indices.size()),
                                    GL_UNSIGNED_INT,
                                    0,
                                    modelInstances.modelMatricesList[pivot].size());
         }
      }
   }
}

void RoadSigns::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor)
{
   signModel->modelShader.use();
   signModel->modelShader.setVec3("light.direction", direction);
   signModel->modelShader.setVec3("light.color", lightColor);
   signModelLP->modelShader.use();
   signModelLP->modelShader.setVec3("light.direction", direction);
   signModelLP->modelShader.setVec3("light.color", lightColor);
}

void RoadSigns::cleanUpBuffers()
{
   modelInstances.clearBuffers();

   for (GLuint vao : signVAOs)
   {
      glDeleteVertexArrays(1, &vao);
   }

   for (GLuint vao : signLPVAOs)
   {
      glDeleteVertexArrays(1, &vao);
   }
}
