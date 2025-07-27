#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef GEOMETRY_CLASS
#define GEOMETRY_CLASS

enum ShaderType{
   VERTEX,
   FRAGMENT,
   GEOMETRY
};

class Shader{
   public:
      GLuint shaderProgram;
      Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
      void use();
      void setBool(const std::string &name, bool val) const;
      void setInt(const std::string &name, int val) const;
      void setFloat(const std::string &name, float val) const;
      void setVec2(const std::string &name, glm::vec2 val) const;
      void setVec3(const std::string &name, glm::vec3 val) const;
      void setVec4(const std::string &name, glm::vec4 val) const;
      void setMat4(const std::string &name, glm::mat4 val) const;
   private:
      GLuint setUpShader(std::string path, enum ShaderType type);
};

#endif