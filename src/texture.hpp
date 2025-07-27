#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifndef TEXTURE
#define TEXTURE

class Texture{
   public:
      GLuint textureID;
      std::string type;
      std::string path;

      Texture(std::string type, std::string path);

   private:
      void loadTexture();
};

#endif