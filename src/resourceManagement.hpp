#include "shader.hpp"
#include "texture.hpp"
#include "unordered_map"

#ifndef RESOURCE_MANAGEMENT
#define RESOURCE_MANAGEMENT

class ResourceManagement{
   public:
      static std::unordered_map<std::string, Shader> shaders;
      static std::unordered_map<std::string, Texture> textures;
      static Shader getShader(std::string shaderName);
      static Shader loadShader(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile);
      static Texture getTexture(std::string textureName);
      static Texture loadTexture(std::string textureName, const char* fileName, bool isAlpha, bool isGammaCorrected);
      static void clearResources();

   private:
      static Shader loadShaderFromFile(const char* vertexFile, const char* fragmentFile, const char* geometryFile);
      static Texture loadTextureFromFile(const char* fileName, bool isAlpha, bool isGammaCorrected);

};

#endif