#include "resourceManagement.hpp"
#include "../lib/stb_image.h"

std::unordered_map<std::string, Shader> ResourceManagement::shaders;
std::unordered_map<std::string, Texture> ResourceManagement::textures;

Shader ResourceManagement::loadShader(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   if (shaders.find(shaderName) != shaders.end()){
      return shaders[shaderName];
   }
   
   shaders[shaderName] = loadShaderFromFile(vertexFile, fragmentFile, geometryFile);
   return shaders[shaderName];
}

Shader ResourceManagement::getShader(std::string shaderName){
   return shaders[shaderName];
}

Texture ResourceManagement::loadTexture(std::string textureName, const char* fileName, bool isAlpha, bool isGammaCorrected){
   if (textures.find(textureName) != textures.end()){
      return textures[textureName];
   }
   
   textures[textureName] = loadTextureFromFile(fileName, isAlpha, isGammaCorrected);
   return textures[textureName];
}  

Texture ResourceManagement::getTexture(std::string textureName){
   return textures[textureName];
}

Shader ResourceManagement::loadShaderFromFile(const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   std::string vertexSource;
   std::string fragmentSource;
   std::string geometrySource;

   try{
      std::ifstream vertexShaderFile(vertexFile);
      std::ifstream fragmentShaderFile(fragmentFile);
      std::stringstream vertexStringStream, fragmentStringStream;

      vertexStringStream << vertexShaderFile.rdbuf();
      fragmentStringStream << fragmentShaderFile.rdbuf();

      vertexShaderFile.close();
      fragmentShaderFile.close();

      vertexSource = vertexStringStream.str();
      fragmentSource = fragmentStringStream.str();

      if (geometryFile != nullptr){
         std::ifstream geometryShaderFile(geometryFile);
         std::stringstream geometryStringStream;

         geometryStringStream << geometryShaderFile.rdbuf();
         geometryShaderFile.close();

         geometrySource = geometryStringStream.str();
      }

   }
   catch (std::exception e){
      std::cout << "ERROR::SHADER::FAILED_TO_READ_SHADER_FILES" << std::endl;
   }

   Shader shader;
   shader.compileShader(vertexSource.c_str(), 
                        fragmentSource.c_str(), 
                        geometryFile != nullptr ? geometrySource.c_str() : nullptr);

   return shader;
}

Texture ResourceManagement::loadTextureFromFile(const char* fileName, bool isAlpha, bool isGammaCorrected){
   Texture texture;
   if (isGammaCorrected){
      if (isAlpha){
         texture.internalFormat = GL_SRGB_ALPHA;
         texture.imageFormat = GL_SRGB_ALPHA;
      }
      else{
         texture.internalFormat = GL_SRGB;
         texture.imageFormat = GL_SRGB;
      }
   }
   else if (isAlpha){
      texture.internalFormat = GL_RGBA;
      texture.imageFormat = GL_RGBA;
   }

   int width, height, nrChannels;
   unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);

   if (!data){
      std::cout << "ERROR::IMAGE::FAILED_TO_LOAD_IMAGE::IMAGE_NAME::" << fileName << "..." << std::endl;
      return texture;
   }

   texture.generateTexture(width, height, nrChannels, data);
   stbi_image_free(data);
   return texture;
}

void ResourceManagement::clearResources(){
   for (auto &i: shaders){
      glDeleteProgram(i.second.shaderProgram);
   }
   for (auto &i: textures){
      glDeleteTextures(1, &i.second.textureID);
   }
}