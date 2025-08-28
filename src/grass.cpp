#include "grass.hpp"

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
{
   glm::vec3 c = a + (b - a) * t;
   return c;
}

float getRandomAngle(float value)
{
   float angle = 3.0f * std::abs(std::sin(value)) - 3.0f * std::abs(std::sin(value * 2)) + 3.0f * std::abs(std::sin(value * 4));
   if (angle >= 3.14159265)
   {
      angle -= 3.14159265;
   }

   return glm::degrees(angle);
}

float getRandomScale(float value)
{
   float scale = 0.5f * std::abs(std::sin(value)) + 0.25f * std::abs(std::sin(value * 2)) + 0.75f * std::abs(std::sin(value * 4));
   scale = std::pow(scale, 3);
   scale = glm::clamp(scale, 0.5f, 1.5f);
   return scale;
}

void GrassBlades::generateGrassModels()
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA1, prevB1, prevA2, prevB2;

   glm::vec3 A1, B1, C1, D1; // the left side
   glm::vec3 A2, B2, C2, D2; // the right side

   glm::vec3 normal1, normal2;
   glm::vec3 midPoint1, midPoint2;

   float segmentLength1, segmentLength2;
   float angle;

   int elements = 0;
   int lastIndex = 0;

   std::vector<glm::mat4> modelMatrices;
   glm::mat4 modelMatrix;
   glm::vec3 pivot;

   for (int i = 0; i < generatedPath.size(); ++i)
   {
      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0)
      {
         A1 = w * (pathWidth / 2 + grassPathWidth + offset) + generatedPath[i];
         B1 = w * (pathWidth / 2 + offset) + generatedPath[i];
         A2 = -w * (pathWidth / 2 + offset) + generatedPath[i];
         B2 = -w * (pathWidth / 2 + grassPathWidth + offset) + generatedPath[i];
      }
      else
      {
         A1 = prevA1;
         B1 = prevB1;
         A2 = prevA2;
         B2 = prevB2;
      }

      C1 = w * (pathWidth / 2 + grassPathWidth + offset) + generatedPath[i + 1];
      D1 = w * (pathWidth / 2 + offset) + generatedPath[i + 1];
      C2 = -w * (pathWidth / 2 + offset) + generatedPath[i + 1];
      D2 = -w * (pathWidth / 2 + grassPathWidth + offset) + generatedPath[i + 1];

      // Save point C1, D1 of the current iteration.
      prevA1 = C1;
      prevB1 = D1;
      prevA2 = C2;
      prevB2 = D2;

      normal1 = glm::normalize(glm::cross(C1 - A1, B1 - A1));

      // Left side
      for (int x = 1; x < grassWidthSize; ++x)
      {
         for (int z = 1; z < grassHeightSize; ++z)
         {
            float tx = static_cast<float>(x) / static_cast<float>(grassWidthSize);
            float tz = static_cast<float>(z) / static_cast<float>(grassHeightSize);

            glm::vec3 P1 = lerp(B1, D1, tx);
            glm::vec3 P2 = lerp(A1, C1, tx);
            glm::vec3 grassPos = lerp(P1, P2, tz);

            float distanceToRoad = pointToSegmentDistance(grassPos, generatedPath[i], generatedPath[i + 1]);
            float noiseValue = noise.getNoise(grassPos.x * noiseScale, 0.0, grassPos.z * noiseScale);
            float multiplierValue = getNoiseMultiplierByDistance(grassPathWidth / 2, distanceToRoad);
            grassPos.y += multiplierValue * noiseValue * amplitude;

            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, grassPos);
            modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, -1.0f, 0.0f), normal1));
            modelMatrix = glm::rotate(modelMatrix, getRandomAngle(noiseValue * amplitude), glm::vec3(0.0f, 1.0f, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(getRandomScale(noiseValue),
                                                            getRandomScale(noiseValue),
                                                            getRandomScale(noiseValue)));
            modelMatrices.push_back(modelMatrix);
         }
      }

      if (elements++ >= partitionSize)
      {
         pivot = generatedPath[lastIndex + (i - lastIndex) / 2];
         modelInstances1.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatrices);
         modelInstances1.insertGridMap({pivot.x, pivot.y, pivot.z});
         modelMatrices.clear();

         lastIndex = i;
         elements = 0;
      }
   }

   if (!modelMatrices.empty())
   {
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];
      modelInstances1.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatrices);
      modelInstances1.insertGridMap({pivot.x, pivot.y, pivot.z});
   }
}

void GrassBlades::setUp()
{
   for (const auto &partition : modelInstances1.modelMatricesList)
   {
      GLuint grassPartitionBuffer;
      glGenBuffers(1, &grassPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, grassPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelInstances1.insertVBO(partition.first, grassPartitionBuffer);
   }

   for (unsigned int i = 0; i < grassBladeModel1->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModel1->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      grassVAOs.push_back(meshVAO);

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

   for (unsigned int i = 0; i < grassBladeModelLP1->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModelLP1->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      grassLPVAOs.push_back(meshVAO);

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

void GrassBlades::render(glm::mat4 view, glm::mat4 projection, Camera &camera)
{
   grassBladeModel1->modelShader.use();
   grassBladeModel1->modelShader.setMat4("view", view);
   grassBladeModel1->modelShader.setMat4("projection", projection);
   grassBladeModel1->modelShader.setVec3("viewPos", camera.cameraPos);

   grassBladeModelLP1->modelShader.use();
   grassBladeModelLP1->modelShader.setMat4("view", view);
   grassBladeModelLP1->modelShader.setMat4("projection", projection);
   grassBladeModelLP1->modelShader.setVec3("viewPos", camera.cameraPos);

   float length;
   std::vector<std::array<float, 3>> nearByPivots;
   modelInstances1.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z},
                                     maxRenderDistance,
                                     nearByPivots);

   for (auto &pivot : nearByPivots)
   {
      if (camera.isInFrustum(glm::vec3(pivot[0], pivot[1], pivot[2]), 100.0f))
      {
         length = glm::distance(camera.cameraPos, glm::vec3(pivot[0], pivot[1], pivot[2]));
         if (length < renderDistance)
         {
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModel1->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModel1->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelInstances1.vbos[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModel1->meshes[meshIndex].bindTextures(grassBladeModel1->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModel1->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelInstances1.modelMatricesList[pivot].size());
            }
         }
         else
         {
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModelLP1->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModelLP1->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelInstances1.vbos[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModelLP1->meshes[meshIndex].bindTextures(grassBladeModelLP1->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModelLP1->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelInstances1.modelMatricesList[pivot].size());
            }
         }
      }
   }
}

void GrassBlades::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor)
{
   grassBladeModel1->modelShader.use();
   grassBladeModel1->modelShader.setVec3("light.direction", direction);
   grassBladeModel1->modelShader.setVec3("light.color", lightColor);
   grassBladeModelLP1->modelShader.use();
   grassBladeModelLP1->modelShader.setVec3("light.direction", direction);
   grassBladeModelLP1->modelShader.setVec3("light.color", lightColor);
}

void GrassBlades::setWindParameters(float time, glm::vec3 windDirection, float windStrength)
{
   grassBladeModel1->modelShader.use();
   grassBladeModel1->modelShader.setFloat("time", time);
   grassBladeModel1->modelShader.setVec3("windDirection", windDirection);
   grassBladeModel1->modelShader.setFloat("windStrength", windStrength);

   grassBladeModelLP1->modelShader.use();
   grassBladeModelLP1->modelShader.setFloat("time", time);
   grassBladeModelLP1->modelShader.setVec3("windDirection", windDirection);
   grassBladeModelLP1->modelShader.setFloat("windStrength", windStrength);
}

void GrassBlades::unbindBuffers()
{
   for (auto &i : grassVAOs)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : grassLPVAOs)
   {
      glDeleteVertexArrays(1, &i);
   }
}