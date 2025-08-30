#include <string>
#include <array>
#include "glad/glad.h"
#include <iostream>
#include <string>
#include "shader.hpp"

#ifndef CUBEMAP
#define CUBEMAP
class Cubemap
{
public:
   Cubemap();
   ~Cubemap()
   {
      glDeleteVertexArrays(1, &vao);
      glDeleteBuffers(1, &vbo);
   }

   void draw(Shader &shader, glm::mat4 projection, glm::mat4 view);

private:
   std::array<float, 36 * 3> cubeSkybox = {
       -1.0f, 1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       -1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, 1.0f};

   std::array<std::string, 6> images = {"../assets/Skybox/right.jpg",
                                        "../assets/Skybox/left.jpg",
                                        "../assets/Skybox/top.jpg",
                                        "../assets/Skybox/bottom.jpg",
                                        "../assets/Skybox/front.jpg",
                                        "../assets/Skybox/back.jpg"};

   GLuint vao, vbo;
   GLuint textureID;

   void setUpCubemap();
   GLuint generateCubemap();
};

#endif