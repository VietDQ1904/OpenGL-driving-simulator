#include <vector>
#include "perlinNoise.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef TERRAIN
#define TERRAIN

class Terrain{
   public:
      const int chunkRenderDistance = 3;
      const int xMapChunks = 10, yMapChunks = 10;
      const int chunkWidth = 127, chunkHeight = 127;
      const int numIndices = chunkWidth * chunkHeight * 6;

      int gridPosX = 0, gridPosY = 0;
      const float originX = (chunkWidth * xMapChunks) / 2.0f - chunkWidth / 2.0f;
      const float originY = (chunkHeight * yMapChunks) / 2.0f - chunkHeight / 2.0f;
      
      int octaves = 5;
      float meshHeight = 8.0f;
      float noiseScale = 128.0f;
      float persistance = 0.5f;
      float lacunarity = 2.0f; 

      std::vector<unsigned int> mapChunks;
      PerlinNoise perlinNoise;

      glm::mat4 projection;
      glm::mat4 view;
      glm::mat4 model;

      Terrain();
      void render(Shader &shader, Camera &camera);

   private:
      void generateChunks(GLuint &vao, int offsetX, int offsetY);
      std::vector<int> generateIndices();
      std::vector<float> generateVertices(const std::vector<float> &noiseMap);
      std::vector<float> generateNoiseMap(int offsetX, int offsetY);
      std::vector<float> generateNormals(const std::vector<int> &indices, const std::vector<float> &vertices);
      std::vector<float> generateTextures();

};

#endif