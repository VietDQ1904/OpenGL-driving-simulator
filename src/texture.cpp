#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

Texture::Texture() : width(0),
                     height(0),
                     wrapS(GL_REPEAT),
                     wrapT(GL_REPEAT),
                     filterMin(GL_LINEAR_MIPMAP_LINEAR),
                     filterMax(GL_LINEAR),
                     imageFormat(GL_RGB),
                     internalFormat(GL_RGB)
{
}

void Texture::generateTexture(int width, int height, int nrChannels, unsigned char *data)
{
   glGenTextures(1, &this->textureID);
   glBindTexture(GL_TEXTURE_2D, this->textureID);
   glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

   glGenerateMipmap(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bindTexture()
{
   glBindTexture(GL_TEXTURE_2D, this->textureID);
}
