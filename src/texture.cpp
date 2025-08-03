#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

Texture::Texture(std::string type, std::string path): 
   type(type),
   path(path)  
{
   loadTexture();
}

void Texture::loadTexture(){

   int width, height, nrChannels;
   stbi_set_flip_vertically_on_load(true);
   unsigned char *data = stbi_load(this->path.c_str(), &width, &height, &nrChannels, 0);

   if (data){
      GLenum formatStandard;
      formatStandard = (nrChannels == 1) ? GL_RED :
                     (nrChannels == 3) ? GL_RGB :
                     (nrChannels == 4) ? GL_RGBA : 0;
     

      GLenum format = (nrChannels == 1) ? GL_RED :
                        (nrChannels == 3) ? GL_RGB :
                        (nrChannels == 4) ? GL_RGBA : 0;
               
      glGenTextures(1, &this->textureID);
      glBindTexture(GL_TEXTURE_2D, this->textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, formatStandard, width, height, 0, format, GL_UNSIGNED_BYTE, data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glGenerateMipmap(GL_TEXTURE_2D);
      stbi_image_free(data);
   }
   else{
      std::cout << "ERROR::IMAGE::FAILED TO LOAD IMAGE " << path << std::endl;
   }

}
