#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SHADER
#define SHADER

enum ShaderType
{
   VERTEX,
   FRAGMENT,
   GEOMETRY,
   PROGRAM
};

class Shader
{
public:
   GLuint shaderProgram;
   Shader() {}
   Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath);
   // Shader(const char* vertexSource, const char* fragmentSource, const char* geometrySource);
   void use();
   void compileShader(const char *vertexSource, const char *fragmentSource, const char *geometrySource);
   void setBool(const std::string &name, bool val) const;
   void setInt(const std::string &name, int val) const;
   void setFloat(const std::string &name, float val) const;
   void setVec2(const std::string &name, glm::vec2 val) const;
   void setVec3(const std::string &name, glm::vec3 val) const;
   void setVec4(const std::string &name, glm::vec4 val) const;
   void setMat4(const std::string &name, glm::mat4 val) const;

private:
   GLuint setUpShader(std::string path, enum ShaderType type);
   void checkForErrors(GLuint shader, ShaderType type);
};

#endif