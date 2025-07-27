#include "terrain.hpp"
#include <math.h>
#include <algorithm>

Terrain::Terrain(){
   mapChunks.resize(this->xMapChunks * this->yMapChunks);
   
   for (unsigned int y = 0; y < yMapChunks; ++y){
      for (unsigned int x = 0; x < xMapChunks; ++x){
         generateChunks(mapChunks[x + y * xMapChunks], x, y);
      }
   }

}

void Terrain::generateChunks(GLuint &vao, int offsetX, int offsetY){
   std::vector<int> indices;
   std::vector<float> noiseMap;
   std::vector<float> vertices;
   std::vector<float> normals;
   
   indices = generateIndices();
   noiseMap = generateNoiseMap(offsetX, offsetY);
   vertices = generateVertices(noiseMap);
   //normals = generateNormals(indices, vertices);

   GLuint vbo[1], ebo;
   
   glGenBuffers(1, vbo);
   glGenBuffers(1, &ebo);
   glGenVertexArrays(1, &vao);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
   glEnableVertexAttribArray(0);

   // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
   // glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);

   // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
   // glEnableVertexAttribArray(1);

}

std::vector<float> Terrain::generateNoiseMap(int offsetX, int offsetY){
   std::vector<float> noiseValues; 
   std::vector<float> normalizedNoiseValues;

   float amplitude = 1.0f;
   float frequency = 1.0f;
   float maxHeight = 0.0f;

   for (unsigned int i = 0; i < octaves; ++i){
      maxHeight += amplitude;
      amplitude *= persistance;
   }

   for (unsigned int y = 0; y < chunkHeight; ++y){
      for (unsigned int x = 0; x < chunkWidth; ++x){

         amplitude = 1.0f;
         frequency = 1.0f;
         float noiseHeight = 0.0f;

         for (unsigned int i = 0; i < octaves; ++i){
            float xSample = (x + offsetX * (chunkWidth - 1)) / noiseScale * frequency;
            float ySample = (y + offsetY * (chunkHeight - 1)) / noiseScale * frequency;
            
            double perlinValue = perlinNoise.getNoise(xSample, ySample, 0.5f);
            noiseHeight += static_cast<float>(perlinValue) * amplitude;

         }

         noiseValues.push_back(noiseHeight);
      }
   }

   for (unsigned int y = 0; y < chunkHeight; ++y){
      for (unsigned int x = 0; x < chunkWidth; ++x){
         normalizedNoiseValues.push_back((noiseValues[x + y * chunkWidth] + 1) / maxHeight);
      }
   }

   return normalizedNoiseValues;
}

std::vector<float> Terrain::generateVertices(const std::vector<float> &noiseMap){
   std::vector<float> vertices;

   for (unsigned int y = 0; y < chunkHeight; ++y){
      for (unsigned int x = 0; x < chunkWidth; ++x){
         vertices.push_back(x);

         float easedNoise = std::pow(noiseMap[x + y * chunkWidth] * 1.1f, 3);
         vertices.push_back(std::fmax(easedNoise * meshHeight, 0.05f * meshHeight));
         vertices.push_back(y);
      }
   }

   return vertices;
}

std::vector<int> Terrain::generateIndices(){
   std::vector<int> indices;
   
   for (unsigned int y = 0; y < chunkHeight; ++y){
      for (unsigned int x = 0; x < chunkWidth; ++x){
         int pos = x + y * chunkWidth;
         
         if (x == chunkWidth - 1 || y == chunkHeight - 1){
            // No indices for right & top edge.
            continue;
         }
         else{
            // Top-left triangle
            indices.push_back(pos + chunkWidth);
            indices.push_back(pos);
            indices.push_back(pos + chunkWidth + 1);

            // Bottom-right triangle
            indices.push_back(pos + 1);
            indices.push_back(pos + chunkWidth + 1);
            indices.push_back(pos);
         }

      }
   }

   return indices;
}

std::vector<float> Terrain::generateNormals(const std::vector<int> &indices, const std::vector<float> &vertices){
   int pos;
   glm::vec3 normal;
   std::vector<float> normals;
   std::vector<glm::vec3> verts;

   assert(vertices.size() % 3 == 0);
   assert(indices.size() % 3 == 0);
   std::cout << vertices.size() << " " << indices.size() << std::endl;

   for (int i = 0; i < indices.size(); i += 3){

      for (int j = 0; j < indices.size(); ++j){
         pos = indices[i + j] * 3;
         verts.push_back(glm::vec3(vertices[pos], vertices[pos + 1], vertices[pos + 2]));
      }

      glm::vec3 u = verts[i + 1] - verts[i];
      glm::vec3 v = verts[i + 2] - verts[i];

      normal = glm::normalize(-glm::cross(u, v));
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);

   }

   return normals;
}

std::vector<float> Terrain::generateTextures(){
   std::vector<float> textures;

   return textures;
}

void Terrain::render(Shader &shader, Camera &camera){
   gridPosX = (int)(camera.cameraPos.x - originX) / chunkWidth + xMapChunks / 2;
   gridPosY = (int)(camera.cameraPos.z - originY) / chunkHeight + yMapChunks / 2;

   for (int y = 0; y < yMapChunks; ++y){
      for (int x = 0; x < xMapChunks; ++x){
         

         if (std::abs(gridPosX - x) <= chunkRenderDistance && std::abs(y - gridPosY) <= chunkRenderDistance){
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-chunkWidth / 2.0f + (float) (chunkWidth - 1) * x,
                                                    -0.0f,
                                                    -chunkHeight / 2.0f + (float) (chunkHeight - 1) * y));
            shader.setMat4("model", model);
            
            glBindVertexArray(mapChunks[x + y * xMapChunks]);
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);            

         }

      }
   }

}

