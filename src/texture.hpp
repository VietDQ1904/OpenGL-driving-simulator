#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifndef TEXTURE
#define TEXTURE

class Texture{
   public:
      GLuint textureID;
      GLuint Wrap_S; 
      GLuint Wrap_T;
      GLuint Filter_Min; 
      GLuint Filter_Max;
      
      std::string type;
      std::string path;


      Texture(std::string type, std::string path);
      void bindTexture();

   private:
      void loadTexture();
};

#endif