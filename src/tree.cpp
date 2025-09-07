#include "tree.hpp"

void Trees::generateModels()
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA1, prevB1, prevA2, prevB2;

   glm::vec3 A1, B1, C1, D1; // the left side
   glm::vec3 A2, B2, C2, D2; // the right side

   glm::vec3 normal1, normal2;
   glm::mat4 modelMatrix;
   std::vector<glm::mat4> modelMatrices;
   glm::vec3 pivot;

   std::mt19937 engine(seed);
   std::vector<bool> plantLeftTrees(generatedPath.size(), false);
   std::vector<bool> plantRightTrees(generatedPath.size(), false);
   bool plantLeft = true;
   bool plantRight = true;

   for (size_t i = 2; i < generatedPath.size() - 1; ++i)
   {
      // Check if a tree isn't planted on the last two tiles.
      if (plantLeftTrees[i - 1] || plantLeftTrees[i - 2])
      {
         plantLeft = false;
      }
      else
      {
         plantLeft = true;
      }

      if (plantRightTrees[i - 1] || plantRightTrees[i - 2])
      {
         plantRight = false;
      }
      else
      {
         plantRight = true;
      }

      if (plantLeft && getRandomIntegerNumber(engine, 0, 100, seed) <= spawnChancePercentage)
      {
         plantLeftTrees[i] = true;
      }

      if (plantRight && getRandomIntegerNumber(engine, 0, 100, seed) <= spawnChancePercentage)
      {
         plantRightTrees[i] = true;
      }
   }

   int elements = 0;
   int lastIndex = 0;

   for (size_t i = 2; i < generatedPath.size() - 1; ++i)
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

      prevA1 = C1;
      prevB1 = D1;
      prevA2 = C2;
      prevB2 = D2;

      normal1 = glm::normalize(glm::cross(C1 - A1, B1 - A1));
      normal2 = glm::normalize(glm::cross(C2 - A2, B2 - A2));

      if (plantLeftTrees[i])
      {
         float xPos = getRandomFloatNumber(engine, 0.15f, 0.55f, seed);
         float zPos = getRandomFloatNumber(engine, 0.15f, 0.55f, seed);
         glm::vec3 P1 = lerp(B1, D1, xPos);
         glm::vec3 P2 = lerp(A1, C1, xPos);
         glm::vec3 treePos = lerp(P1, P2, zPos);

         float distanceToRoad = pointToSegmentDistance(treePos, generatedPath[i], generatedPath[i + 1]);
         float noiseValue = noise.getNoise(treePos.x * noiseScale, 0.0, treePos.z * noiseScale);
         float multiplierValue = getNoiseMultiplierByDistance(grassPathWidth / 2, distanceToRoad);
         treePos.y += multiplierValue * noiseValue * amplitude;

         modelMatrix = glm::mat4(1.0f);
         modelMatrix = glm::translate(modelMatrix, treePos);
         modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, -1.0f, 0.0f), normal1));
         modelMatrix = glm::rotate(modelMatrix, getRandomFloatNumber(engine, -180.0f, 180.0f, seed),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
         modelMatrices.push_back(modelMatrix);
      }

      if (plantRightTrees[i])
      {
         float xPos = getRandomFloatNumber(engine, 0.15f, 0.55f, seed);
         float zPos = getRandomFloatNumber(engine, 0.15f, 0.55f, seed);
         glm::vec3 P1 = lerp(B2, D2, xPos);
         glm::vec3 P2 = lerp(A2, C2, xPos);
         glm::vec3 treePos = lerp(P2, P1, zPos);

         float distanceToRoad = pointToSegmentDistance(treePos, generatedPath[i], generatedPath[i + 1]);
         float noiseValue = noise.getNoise(treePos.x * noiseScale, 0.0, treePos.z * noiseScale);
         float multiplierValue = getNoiseMultiplierByDistance(grassPathWidth / 2, distanceToRoad);
         treePos.y += multiplierValue * noiseValue * amplitude;

         modelMatrix = glm::mat4(1.0f);
         modelMatrix = glm::translate(modelMatrix, treePos);
         modelMatrix *= glm::toMat4(glm::rotation(glm::vec3(0.0f, -1.0f, 0.0f), normal2));
         modelMatrix = glm::rotate(modelMatrix, getRandomFloatNumber(engine, -180.0f, 180.0f, seed),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
         modelMatrices.push_back(modelMatrix);
      }

      if (elements++ >= partitionSize)
      {
         pivot = generatedPath[lastIndex + (i - lastIndex) / 2];
         instances.insertGridMap({pivot.x, pivot.y, pivot.z});
         modelMatrices1[{pivot.x, pivot.y, pivot.z}] = modelMatrices;

         modelMatrices.clear();
         lastIndex = i;
         elements = 0;
      }
   }

   if (!modelMatrices1.empty())
   {
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];
      modelMatrices1[{pivot.x, pivot.y, pivot.z}] = modelMatrices;
      instances.insertGridMap({pivot.x, pivot.y, pivot.z});
   }
}

void Trees::setUp()
{
   for (const auto &partition : modelMatrices1)
   {
      GLuint grassPartitionBuffer;
      glGenBuffers(1, &grassPartitionBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, grassPartitionBuffer);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(glm::mat4), partition.second.data(), GL_STATIC_DRAW);
      modelVBOs1[partition.first] = grassPartitionBuffer;
   }

   for (unsigned int i = 0; i < treeModel->meshes.size(); ++i)
   {
      GLuint meshVAO = treeModel->meshes[i].vao;
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

   for (unsigned int i = 0; i < treeLPModel->meshes.size(); ++i)
   {
      GLuint meshVAO = treeLPModel->meshes[i].vao;
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

   for (unsigned int i = 0; i < treeLP2Model->meshes.size(); ++i)
   {
      GLuint meshVAO = treeLP2Model->meshes[i].vao;
      size_t matrixSegment = sizeof(glm::vec4);
      glBindVertexArray(meshVAO);
      modelLP2VAOs1.push_back(meshVAO);

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

void Trees::loadResources()
{
   treeModel = std::make_unique<Model>("../assets/Tree/tree.obj");
   treeLPModel = std::make_unique<Model>("../assets/Tree/treeLP.obj");
   treeLP2Model = std::make_unique<Model>("../assets/Tree/treeLP2.obj");

   treeModel->loadShader("TreeModelShader",
                         "../src/Shaders/instanceModel.vert",
                         "../src/Shaders/instanceModel.frag",
                         nullptr);

   treeModel->loadTextures();

   treeLPModel->loadShader("TreeModelLPShader",
                           "../src/Shaders/instanceModel.vert",
                           "../src/Shaders/instanceModel.frag",
                           nullptr);

   treeLPModel->loadTextures();

   treeLP2Model->loadShader("TreeModelLP2Shader",
                            "../src/Shaders/instanceModel.vert",
                            "../src/Shaders/instanceModel.frag",
                            nullptr);

   treeLP2Model->loadTextures();

   this->setUp();
}

void Trees::render(glm::mat4 view, glm::mat4 projection, Camera &camera)
{
   treeModel->modelShader.use();
   treeModel->modelShader.setMat4("view", view);
   treeModel->modelShader.setMat4("projection", projection);
   treeModel->modelShader.setVec3("viewPos", camera.cameraPos);

   treeLPModel->modelShader.use();
   treeLPModel->modelShader.setMat4("view", view);
   treeLPModel->modelShader.setMat4("projection", projection);
   treeLPModel->modelShader.setVec3("viewPos", camera.cameraPos);

   treeLP2Model->modelShader.use();
   treeLP2Model->modelShader.setMat4("view", view);
   treeLP2Model->modelShader.setMat4("projection", projection);
   treeLP2Model->modelShader.setVec3("viewPos", camera.cameraPos);

   float length;
   std::vector<std::array<float, 3>> nearByPivots;
   instances.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z},
                               maxRenderDistance,
                               nearByPivots);

   for (auto &pivot : nearByPivots)
   {
      length = glm::distance(camera.cameraPos, glm::vec3(pivot[0], pivot[1], pivot[2]));
      if (length < renderDistance)
      {
         treeModel->modelShader.use();
         for (unsigned int meshIndex = 0; meshIndex < treeModel->meshes.size(); ++meshIndex)
         {
            glBindVertexArray(treeModel->meshes[meshIndex].vao);
            glBindBuffer(GL_ARRAY_BUFFER, modelVBOs1[pivot]);

            size_t matrixSegment = sizeof(glm::vec4);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

            treeModel->meshes[meshIndex].bindTextures(treeModel->modelShader);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<unsigned int>(treeModel->meshes[meshIndex].indices.size()),
                                    GL_UNSIGNED_INT,
                                    0,
                                    modelMatrices1[pivot].size());
         }
         glBindVertexArray(0);
      }

      else if (length < mediumRenderDistance)
      {
         treeLPModel->modelShader.use();
         for (unsigned int meshIndex = 0; meshIndex < treeLPModel->meshes.size(); ++meshIndex)
         {
            glBindVertexArray(treeLPModel->meshes[meshIndex].vao);
            glBindBuffer(GL_ARRAY_BUFFER, modelVBOs1[pivot]);

            size_t matrixSegment = sizeof(glm::vec4);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

            treeLPModel->meshes[meshIndex].bindTextures(treeLPModel->modelShader);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<unsigned int>(treeLPModel->meshes[meshIndex].indices.size()),
                                    GL_UNSIGNED_INT,
                                    0,
                                    modelMatrices1[pivot].size());
         }
         glBindVertexArray(0);
      }

      else
      {
         treeLP2Model->modelShader.use();
         for (unsigned int meshIndex = 0; meshIndex < treeLP2Model->meshes.size(); ++meshIndex)
         {
            glBindVertexArray(treeLP2Model->meshes[meshIndex].vao);
            glBindBuffer(GL_ARRAY_BUFFER, modelVBOs1[pivot]);

            size_t matrixSegment = sizeof(glm::vec4);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)0);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(matrixSegment));
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(2 * matrixSegment));
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * matrixSegment, (void *)(3 * matrixSegment));

            treeLP2Model->meshes[meshIndex].bindTextures(treeLP2Model->modelShader);
            glDrawElementsInstanced(GL_TRIANGLES,
                                    static_cast<unsigned int>(treeLP2Model->meshes[meshIndex].indices.size()),
                                    GL_UNSIGNED_INT,
                                    0,
                                    modelMatrices1[pivot].size());
         }
         glBindVertexArray(0);
      }
   }
}

void Trees::setEnvironmentLighting(glm::vec3 direction, glm::vec3 lightColor)
{
   treeModel->modelShader.use();
   treeModel->modelShader.setVec3("light.direction", direction);
   treeModel->modelShader.setVec3("light.color", lightColor);
   treeLPModel->modelShader.use();
   treeLPModel->modelShader.setVec3("light.direction", direction);
   treeLPModel->modelShader.setVec3("light.color", lightColor);
   treeLP2Model->modelShader.use();
   treeLP2Model->modelShader.setVec3("light.direction", direction);
   treeLP2Model->modelShader.setVec3("light.color", lightColor);
}

void Trees::unbindBuffers()
{
   for (auto &i : modelVBOs1)
   {
      glDeleteBuffers(1, &i.second);
   }

   for (auto &i : modelVAOs1)
   {
      glDeleteVertexArrays(1, &i);
   }

   for (auto &i : modelLP2VAOs1)
   {
      glDeleteVertexArrays(1, &i);
   }

   instances.clearBuffers();
}
