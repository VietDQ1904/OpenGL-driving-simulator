#include "roadPath.hpp"
#include "camera.hpp"

void Road::generateVertices(Physics &simulation)
{
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA, prevB;

   glm::vec3 A;
   glm::vec3 B;
   glm::vec3 C;
   glm::vec3 D;

   glm::vec3 normal;

   float segmentLength;
   glm::vec3 pivot;

   std::vector<float> subVertices;
   std::vector<int> subIndices;
   int elements = 0;
   int lastIndex = 0;
   int startIndex = 0;
   int totalVertices = 0;
   for (int i = 0; i < generatedPath.size() - 1; ++i)
   {
      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0)
      { // Only calculate A and B for the first iteration
         A = w * (roadPathWidth / 2) + generatedPath[i];
         B = -w * (roadPathWidth / 2) + generatedPath[i];
      }
      else
      { // Assign the previous points of C and D.
         A = prevA;
         B = prevB;
      }

      C = w * (roadPathWidth / 2) + generatedPath[i + 1];
      D = -w * (roadPathWidth / 2) + generatedPath[i + 1];

      // Save point C, D of the current iteration.
      prevA = C;
      prevB = D;

      normal = glm::normalize(glm::cross(C - A, B - A));
      segmentLength = glm::length(generatedPath[i + 1] - generatedPath[i]);

      // 1st vertex
      subVertices.push_back(A.x);
      subVertices.push_back(A.y);
      subVertices.push_back(A.z);
      subVertices.push_back(normal.x);
      subVertices.push_back(normal.y);
      subVertices.push_back(normal.z);
      subVertices.push_back(0.0f);
      subVertices.push_back(0.2f * (roadPathWidth));

      // 2nd vertex
      subVertices.push_back(B.x);
      subVertices.push_back(B.y);
      subVertices.push_back(B.z);
      subVertices.push_back(normal.x);
      subVertices.push_back(normal.y);
      subVertices.push_back(normal.z);
      subVertices.push_back(0.0f);
      subVertices.push_back(0.0f);

      // 3rd vertex
      subVertices.push_back(C.x);
      subVertices.push_back(C.y);
      subVertices.push_back(C.z);
      subVertices.push_back(normal.x);
      subVertices.push_back(normal.y);
      subVertices.push_back(normal.z);
      subVertices.push_back(0.2f * (segmentLength));
      subVertices.push_back(0.2f * (roadPathWidth));

      // 4th vertex
      subVertices.push_back(D.x);
      subVertices.push_back(D.y);
      subVertices.push_back(D.z);
      subVertices.push_back(normal.x);
      subVertices.push_back(normal.y);
      subVertices.push_back(normal.z);
      subVertices.push_back(0.2f * (segmentLength));
      subVertices.push_back(0.0f);

      startIndex = totalVertices;
      subIndices.push_back(startIndex + 0);
      subIndices.push_back(startIndex + 1);
      subIndices.push_back(startIndex + 2);

      subIndices.push_back(startIndex + 1);
      subIndices.push_back(startIndex + 2);
      subIndices.push_back(startIndex + 3);

      totalVertices += 4;

      if (elements++ >= partitionSize)
      {
         pivot = generatedPath[lastIndex + (i - lastIndex) / 2];

         verticesWithIndices.insertGridMap({pivot.x, pivot.y, pivot.z});
         verticesWithIndices.insertVertices({pivot.x, pivot.y, pivot.z}, subVertices);
         verticesWithIndices.insertIndices({pivot.x, pivot.y, pivot.z}, subIndices);

         subVertices.clear();
         subIndices.clear();
         lastIndex = i;
         elements = 0;
         totalVertices = 0;
      }

      // Create a rigid body.
      simulation.createRigidBody(A, B, C, D, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
   }

   if (!subIndices.empty())
   {
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];
      verticesWithIndices.insertGridMap({pivot.x, pivot.y, pivot.z});
      verticesWithIndices.insertVertices({pivot.x, pivot.y, pivot.z}, subVertices);
      verticesWithIndices.insertIndices({pivot.x, pivot.y, pivot.z}, subIndices);
   }
}

void Road::setUp()
{
   for (auto &partition : verticesWithIndices.vertices)
   {
      GLuint vao, vbo, ebo;

      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glGenBuffers(1, &ebo);

      verticesWithIndices.insertVAO(partition.first, vao);
      verticesWithIndices.insertVBO(partition.first, vbo);
      verticesWithIndices.insertEBO(partition.first, ebo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, partition.second.size() * sizeof(float), partition.second.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, verticesWithIndices.indices[partition.first].size() * sizeof(int),
                   verticesWithIndices.indices[partition.first].data(), GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

      glBindVertexArray(0);
   }
}

void Road::render(Shader &shader, Camera &camera)
{

   model = glm::mat4(1.0f);
   shader.setMat4("model", model);

   std::vector<std::array<float, 3>> nearbyPivots;
   verticesWithIndices.findPivotsByRange({camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z}, renderDistance,
                                         nearbyPivots);

   for (auto &pivot : nearbyPivots)
   {
      if (camera.isInFrustum(glm::vec3(pivot[0], pivot[1], pivot[2]), renderDistance))
      {
         glBindVertexArray(verticesWithIndices.vaos[pivot]);
         glDrawElements(GL_TRIANGLES, verticesWithIndices.indices[pivot].size(), GL_UNSIGNED_INT, 0);
      }
   }

   glBindVertexArray(0);
}

void Road::cleanUpBuffers()
{
   verticesWithIndices.clearBuffers();
}
