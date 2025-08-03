#include "cubeMap.hpp"
#include "../lib/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Cubemap::Cubemap(std::string directory, std::array<std::string, 6> files){
   this->textureID = generateCubemap(directory, files);
   setUpCubemap();
}

GLuint Cubemap::generateCubemap(std::string directory, std::array<std::string, 6> files){
   GLuint textureID;
   glGenTextures(1, &textureID);
   glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

   int width, height, nrChannels;
   unsigned char *data;

   // Axis loading order: +x -> -x -> +y -> -y -> +z -> -z
   for (int i = 0; i < files.size(); ++i){
      std::string file = directory + "/" + files[i];
      data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);

      if (!data){
         std::cout << "ERROR::CUBEMAP::FAILED_TO_LOAD_IMAGE\n";
      }
      else{
         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      }

      stbi_image_free(data);
   }

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   return textureID;
}

void Cubemap::setUpCubemap(){
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cubeSkybox.size(), cubeSkybox.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
}

void Cubemap::unbind(){
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
}

void Cubemap::draw(Shader &shader, glm::mat4 projection, glm::mat4 view){
   glDepthFunc(GL_LEQUAL);
   shader.use();

   shader.setMat4("projection", projection);
   shader.setMat4("view", glm::mat4(glm::mat3(view)));
   shader.setInt("skybox", 0);

   glBindVertexArray(vao);
   glActiveTexture(GL_TEXTURE0);

   glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
   glDrawArrays(GL_TRIANGLES, 0, 36);

   glBindVertexArray(0);
   glDepthFunc(GL_LESS);
}

