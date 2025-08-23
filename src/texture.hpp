#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#ifndef TEXTURE
#define TEXTURE

class Texture
{
public:
   GLuint textureID;
   GLuint width, height;
   GLuint wrapS, wrapT;
   GLuint filterMin, filterMax;
   GLuint internalFormat;
   GLuint imageFormat;

   std::string type;
   std::string path;

   Texture();
   void bindTexture();
   void generateTexture(int width, int height, int nrChannels, unsigned char *data);

private:
   void loadTexture();
};

#endif