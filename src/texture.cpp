#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

Texture::Texture(std::string type, std::string path): 
   type(type),
   path(path),
   wrapS(GL_REPEAT), 
   wrapT(GL_REPEAT), 
   filterMin(GL_LINEAR_MIPMAP_LINEAR), 
   filterMax(GL_LINEAR)
{
   loadTexture();
}

Texture::Texture(): 
   width(0),
   height(0),
   wrapS(GL_REPEAT),
   wrapT(GL_REPEAT),
   filterMin(GL_LINEAR_MIPMAP_LINEAR),
   filterMax(GL_LINEAR),
   imageFormat(GL_RGB),
   internalFormat(GL_RGB)
{ 
   glGenTextures(1, &this->textureID);
}

void Texture::loadTexture(){

   int width, height, nrChannels;

   //stbi_set_flip_vertically_on_load(true);
   unsigned char *data = stbi_load(this->path.c_str(), &width, &height, &nrChannels, 0);
   
   if (data){
      GLenum formatStandard;

      if (type == "texture_diffuse"){
         formatStandard = (nrChannels == 1) ? GL_RED :
                     (nrChannels == 3) ? GL_SRGB :
                     (nrChannels == 4) ? GL_SRGB_ALPHA : 0;
      }
      else{
         formatStandard = (nrChannels == 1) ? GL_RED :
                        (nrChannels == 3) ? GL_RGB :
                        (nrChannels == 4) ? GL_RGBA : 0;
      }
      
      GLenum format = (nrChannels == 1) ? GL_RED :
                        (nrChannels == 3) ? GL_RGB :
                        (nrChannels == 4) ? GL_RGBA : 0;
  
               
      glGenTextures(1, &this->textureID);
      glBindTexture(GL_TEXTURE_2D, this->textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, formatStandard, width, height, 0, format, GL_UNSIGNED_BYTE, data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

      glGenerateMipmap(GL_TEXTURE_2D);
      stbi_image_free(data);
   }
   else{
      std::cout << "ERROR::IMAGE::FAILED TO LOAD IMAGE " << path << std::endl;
   }
   
}

void Texture::generateTexture(int width, int height, int nrChannels, unsigned char* data){

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

void Texture::bindTexture(){
   glBindTexture(1, this->textureID);
}
