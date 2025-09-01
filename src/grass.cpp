#include "grass.hpp"

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
{
   glm::vec3 c = a + (b - a) * t;
   return c;
}

float getRandomFloatNumber(std::mt19937 &engine, float start, float end, unsigned int seed)
{
   std::uniform_real_distribution<float> distribution(start, end);
   float rand = distribution(engine);
   return rand;
}

int getRandomIntegerNumber(std::mt19937 &engine, int start, int end, unsigned int seed)
{
   std::uniform_int_distribution<int> distribution(start, end);
   int rand = distribution(engine);
   return rand;
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
   float angle;

   int elements = 0;
   int lastIndex = 0;

   std::vector<glm::mat4> modelMatrices;

   std::vector<glm::mat4> modelMatrices1;
   std::vector<glm::mat4> modelMatrices2;
   std::vector<glm::mat4> modelMatrices3;

   glm::mat4 modelMatrix;
   glm::vec3 pivot;

   std::mt19937 engine(seed);

   for (size_t i = 0; i < generatedPath.size() - 1; ++i)
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
      normal2 = glm::normalize(glm::cross(C2 - A2, B2 - A2));

      for (size_t x = 0; x < grassWidthSize; ++x)
      {
         for (size_t z = 0; z < grassHeightSize; ++z)
         {
            // Left side
            float tx = static_cast<float>(x) / static_cast<float>(grassWidthSize);
            float tz = static_cast<float>(z) / static_cast<float>(grassHeightSize);
            tz /= zScale;

            glm::vec3 P1 = lerp(B1, D1, tx);
            glm::vec3 P2 = lerp(A1, C1, tx);
            glm::vec3 grassPos = lerp(P1, P2, tz);

            float chance = grassMap.getNoise(grassPos.x, grassPos.y, grassPos.z);
            if (getRandomFloatNumber(engine, 0.0f, 1.0f, seed) < chance)
            {
               grassPos.x += getRandomFloatNumber(engine, -3.0f, 3.0f, seed);
               grassPos.z += getRandomFloatNumber(engine, -3.0f, 3.0f, seed);

               float distanceToRoad = pointToSegmentDistance(grassPos, generatedPath[i], generatedPath[i + 1]);
               float noiseValue = noise.getNoise(grassPos.x * noiseScale, 0.0, grassPos.z * noiseScale);
               float multiplierValue = getNoiseMultiplierByDistance(grassPathWidth / 2, distanceToRoad);
               grassPos.y += multiplierValue * noiseValue * amplitude;

               modelMatrix = glm::mat4(1.0f);
               modelMatrix = glm::translate(modelMatrix, grassPos);
               modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, -1.0f, 0.0f), normal1));
               modelMatrix = glm::rotate(modelMatrix, getRandomFloatNumber(engine, -180.0f, 180.0f, seed),
                                         glm::vec3(0.0f, 1.0f, 0.0f));

               float randomScale = getRandomFloatNumber(engine, 0.5f, 1.5f, seed);
               modelMatrix = glm::scale(modelMatrix, glm::vec3(randomScale,
                                                               randomScale,
                                                               randomScale));

               int r = getRandomIntegerNumber(engine, 1, 3, seed);
               switch (r)
               {
               case 1:
                  modelMatrices1.push_back(modelMatrix);
                  break;
               case 2:
                  modelMatrices2.push_back(modelMatrix);
                  break;
               case 3:
                  modelMatrices3.push_back(modelMatrix);
                  break;
               default:
                  break;
               }
            }
            // Right side
            tx = static_cast<float>(x) / static_cast<float>(grassWidthSize);
            tz = static_cast<float>(z) / static_cast<float>(grassHeightSize);
            tz /= zScale;

            P1 = lerp(B2, D2, tx);
            P2 = lerp(A2, C2, tx);
            grassPos = lerp(P2, P1, tz);

            chance = grassMap.getNoise(grassPos.x, grassPos.y, grassPos.z);
            if (getRandomFloatNumber(engine, 0.0f, 1.0f, seed) < chance)
            {
               grassPos.x += getRandomFloatNumber(engine, -3.0f, 3.0f, seed);
               grassPos.z += getRandomFloatNumber(engine, -3.0f, 3.0f, seed);

               float distanceToRoad = pointToSegmentDistance(grassPos, generatedPath[i], generatedPath[i + 1]);
               float noiseValue = noise.getNoise(grassPos.x * noiseScale, 0.0, grassPos.z * noiseScale);
               float multiplierValue = getNoiseMultiplierByDistance(grassPathWidth / 2, distanceToRoad);
               grassPos.y += multiplierValue * noiseValue * amplitude;

               modelMatrix = glm::mat4(1.0f);
               modelMatrix = glm::translate(modelMatrix, grassPos);
               modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, -1.0f, 0.0f), normal1));
               modelMatrix = glm::rotate(modelMatrix, getRandomFloatNumber(engine, -180.0f, 180.0f, seed),
                                         glm::vec3(0.0f, 1.0f, 0.0f));

               float randomScale = getRandomFloatNumber(engine, 0.5f, 1.5f, seed);
               modelMatrix = glm::scale(modelMatrix, glm::vec3(randomScale,
                                                               randomScale,
                                                               randomScale));

               int r = getRandomIntegerNumber(engine, 1, 3, seed);

               switch (r)
               {
               case 1:
                  modelMatrices1.push_back(modelMatrix);
                  break;
               case 2:
                  modelMatrices2.push_back(modelMatrix);
                  break;
               case 3:
                  modelMatrices3.push_back(modelMatrix);
                  break;
               default:
                  break;
               }
            }
         }
      }

      if (elements++ >= partitionSize)
      {
         pivot = generatedPath[lastIndex + (i - lastIndex) / 2];
         modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});

         modelMatricesList1[{pivot.x, pivot.y, pivot.z}] = modelMatrices1;
         modelMatricesList2[{pivot.x, pivot.y, pivot.z}] = modelMatrices2;
         modelMatricesList3[{pivot.x, pivot.y, pivot.z}] = modelMatrices3;

         modelMatrices1.clear();
         modelMatrices2.clear();
         modelMatrices3.clear();

         lastIndex = i;
         elements = 0;
      }
   }

   if (!modelMatrices.empty())
   {
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];
      // modelInstances.insertModelMatrices({pivot.x, pivot.y, pivot.z}, modelMatrices);

      modelMatricesList1[{pivot.x, pivot.y, pivot.z}] = modelMatrices1;
      modelMatricesList2[{pivot.x, pivot.y, pivot.z}] = modelMatrices2;
      modelMatricesList3[{pivot.x, pivot.y, pivot.z}] = modelMatrices3;
      modelInstances.insertGridMap({pivot.x, pivot.y, pivot.z});
   }
}

void GrassBlades::setUp()
{
   for (const auto &partition : modelMatricesList1)
   {
      GLuint grassPartitionBuffer;
      glGenBuffers(1, &grassPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, grassPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelVBOs1[partition.first] = grassPartitionBuffer;
   }

   for (const auto &partition : modelMatricesList2)
   {
      GLuint grassPartitionBuffer;
      glGenBuffers(1, &grassPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, grassPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelVBOs2[partition.first] = grassPartitionBuffer;
   }

   for (const auto &partition : modelMatricesList3)
   {
      GLuint grassPartitionBuffer;
      glGenBuffers(1, &grassPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, grassPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelVBOs3[partition.first] = grassPartitionBuffer;
   }

   for (unsigned int i = 0; i < grassBladeModel1->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModel1->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelVAOs1.push_back(meshVAO);

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
      modelLPVAOs1.push_back(meshVAO);

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

   for (unsigned int i = 0; i < grassBladeModel2->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModel2->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelVAOs2.push_back(meshVAO);

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

   for (unsigned int i = 0; i < grassBladeModelLP2->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModelLP2->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelLPVAOs2.push_back(meshVAO);

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
   for (unsigned int i = 0; i < grassBladeModel3->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModel3->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelVAOs3.push_back(meshVAO);

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

   for (unsigned int i = 0; i < grassBladeModelLP3->meshes.size(); ++i)
   {
      GLuint meshVAO = grassBladeModelLP3->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelLPVAOs3.push_back(meshVAO);

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

   std::vector<std::array<float, 3>> nearByPivots;
   modelInstances.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z},
                                    maxRenderDistance,
                                    nearByPivots);

   for (auto &pivot : nearByPivots)
   {
      if (camera.isInFrustum(glm::vec3(pivot[0], pivot[1], pivot[2]), 100.0f))
      {
         float length = glm::distance(camera.cameraPos, glm::vec3(pivot[0], pivot[1], pivot[2]));
         if (length < renderDistance)
         {
            grassBladeModel1->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModel1->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModel1->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs1[pivot]);

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
                                       modelMatricesList1[pivot].size());
            }
            glBindVertexArray(0);

            grassBladeModel2->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModel2->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModel2->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs2[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModel2->meshes[meshIndex].bindTextures(grassBladeModel2->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModel2->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelMatricesList2[pivot].size());
            }
            glBindVertexArray(0);

            grassBladeModel3->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModel3->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModel3->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs3[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModel3->meshes[meshIndex].bindTextures(grassBladeModel3->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModel3->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelMatricesList3[pivot].size());
            }
         }
         else
         {
            grassBladeModelLP1->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModelLP1->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModelLP1->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs1[pivot]);

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
                                       modelMatricesList1[pivot].size());
            }
            glBindVertexArray(0);

            grassBladeModelLP2->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModelLP2->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModelLP2->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs2[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModelLP2->meshes[meshIndex].bindTextures(grassBladeModelLP2->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModelLP2->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelMatricesList2[pivot].size());
            }
            glBindVertexArray(0);

            grassBladeModelLP3->modelShader.use();
            for (unsigned int meshIndex = 0; meshIndex < grassBladeModelLP3->meshes.size(); ++meshIndex)
            {
               glBindVertexArray(grassBladeModelLP3->meshes[meshIndex].vao);
               glBindBuffer(GL_ARRAY_BUFFER, modelVBOs3[pivot]);

               size_t matrixSegment = sizeof(glm::vec4);
               glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
               glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
               glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
               glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

               grassBladeModelLP3->meshes[meshIndex].bindTextures(grassBladeModelLP3->modelShader);
               glDrawElementsInstanced(GL_TRIANGLES,
                                       static_cast<unsigned int>(grassBladeModelLP3->meshes[meshIndex].indices.size()),
                                       GL_UNSIGNED_INT,
                                       0,
                                       modelMatricesList3[pivot].size());
            }
            glBindVertexArray(0);
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

   grassBladeModel2->modelShader.use();
   grassBladeModel2->modelShader.setVec3("light.direction", direction);
   grassBladeModel2->modelShader.setVec3("light.color", lightColor);
   grassBladeModelLP2->modelShader.use();
   grassBladeModelLP2->modelShader.setVec3("light.direction", direction);
   grassBladeModelLP2->modelShader.setVec3("light.color", lightColor);

   grassBladeModel3->modelShader.use();
   grassBladeModel3->modelShader.setVec3("light.direction", direction);
   grassBladeModel3->modelShader.setVec3("light.color", lightColor);
   grassBladeModelLP3->modelShader.use();
   grassBladeModelLP3->modelShader.setVec3("light.direction", direction);
   grassBladeModelLP3->modelShader.setVec3("light.color", lightColor);
}

void GrassBlades::setWindParameters(float time, glm::vec3 windDirection, float windStrength)
{
   grassBladeModel1->modelShader.use();
   grassBladeModel1->modelShader.setFloat("time", time);
   grassBladeModel1->modelShader.setVec3("windDirection", windDirection);
   grassBladeModel1->modelShader.setFloat("windStrength", windStrength);

   grassBladeModel2->modelShader.use();
   grassBladeModel2->modelShader.setFloat("time", time);
   grassBladeModel2->modelShader.setVec3("windDirection", windDirection);
   grassBladeModel2->modelShader.setFloat("windStrength", windStrength);

   grassBladeModel3->modelShader.use();
   grassBladeModel3->modelShader.setFloat("time", time);
   grassBladeModel3->modelShader.setVec3("windDirection", windDirection);
   grassBladeModel3->modelShader.setFloat("windStrength", windStrength);
}

void GrassBlades::loadResources()
{
   grassBladeModel1 = std::make_unique<Model>("../assets/Grass/grassBladeA.obj");
   grassBladeModel2 = std::make_unique<Model>("../assets/Grass/grassBladeB.obj");
   grassBladeModel3 = std::make_unique<Model>("../assets/Grass/grassBladeC.obj");

   grassBladeModelLP1 = std::make_unique<Model>("../assets/Grass/grassBladeLP_A.obj");
   grassBladeModelLP2 = std::make_unique<Model>("../assets/Grass/grassBladeLP_B.obj");
   grassBladeModelLP3 = std::make_unique<Model>("../assets/Grass/grassBladeLP_C.obj");

   grassBladeModel1->loadTextures();
   grassBladeModel2->loadTextures();
   grassBladeModel3->loadTextures();

   grassBladeModel1->loadShader("GrassBladeShader",
                                "../src/Shaders/grassModel.vert",
                                "../src/Shaders/instanceModel.frag",
                                nullptr);

   grassBladeModel2->loadShader("GrassBladeShader",
                                "../src/Shaders/grassModel.vert",
                                "../src/Shaders/instanceModel.frag",
                                nullptr);

   grassBladeModel3->loadShader("GrassBladeShader",
                                "../src/Shaders/grassModel.vert",
                                "../src/Shaders/instanceModel.frag",
                                nullptr);

   grassBladeModelLP1->loadTextures();
   grassBladeModelLP2->loadTextures();
   grassBladeModelLP3->loadTextures();

   grassBladeModelLP1->loadShader("GrassBladeShaderLP",
                                  "../src/Shaders/instanceModel.vert",
                                  "../src/Shaders/instanceModel.frag",
                                  nullptr);

   grassBladeModelLP2->loadShader("GrassBladeShaderLP",
                                  "../src/Shaders/instanceModel.vert",
                                  "../src/Shaders/instanceModel.frag",
                                  nullptr);

   grassBladeModelLP3->loadShader("GrassBladeShaderLP",
                                  "../src/Shaders/instanceModel.vert",
                                  "../src/Shaders/instanceModel.frag",
                                  nullptr);

   this->setUp();
}

void GrassBlades::unbindBuffers()
{
   for (auto &i : modelVBOs1)
   {
      glDeleteBuffers(1, &i.second);
   }

   for (auto &i : modelVBOs2)
   {
      glDeleteBuffers(1, &i.second);
   }

   for (auto &i : modelVBOs3)
   {
      glDeleteBuffers(1, &i.second);
   }

   for (auto &i : modelVAOs1)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelVAOs2)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelVAOs3)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelLPVAOs1)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelLPVAOs2)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelLPVAOs3)
   {
      glDeleteVertexArrays(1, &i);
   }
}