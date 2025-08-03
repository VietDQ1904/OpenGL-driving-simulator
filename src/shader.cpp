#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath){
   
   shaderProgram = glCreateProgram();
   
   GLuint vertexShader = setUpShader(vertexPath, VERTEX);
   glAttachShader(shaderProgram, vertexShader);

   GLuint fragmentShader = setUpShader(fragmentPath, FRAGMENT);
   glAttachShader(shaderProgram, fragmentShader);

   GLuint geometryShader;
   if (geometryPath != nullptr){
      geometryShader = setUpShader(geometryPath, GEOMETRY);
      glAttachShader(shaderProgram, geometryShader);
   }
   
   glLinkProgram(shaderProgram);

   int success;
   char infoLog[512];

   glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

   if (!success){
      glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
      std::cout << "ERROR::PROGRAM::SHADER_COMPILATION_FAILED\n"
      << infoLog << std::endl;
   }
   
   if (geometryPath != nullptr){
      glDeleteShader(geometryShader);
   }

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

}

GLuint Shader::setUpShader(std::string path, enum ShaderType type){
   std::string shaderSource;
   std::ifstream shaderFile;

   shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

   try{
      shaderFile.open(path);

      std::stringstream shaderStream;
      shaderStream << shaderFile.rdbuf();
      
      shaderFile.close();
      shaderSource = shaderStream.str();
   }
   catch (std::ifstream::failure e){
      std::cout << "ERROR::SHADER::READ_FAILURE::"<< type << std::endl;
   }

   const char *source = shaderSource.c_str();

   int success;
   char infoLog[512];

   GLuint shader;
   std::string shaderType;
   switch (type){
      case 0:
         shader = glCreateShader(GL_VERTEX_SHADER);
         shaderType = "VERTEX_SHADER";
         break;
      case 1:
         shader = glCreateShader(GL_FRAGMENT_SHADER);
         shaderType = "FRAGMENT_SHADER";
         break;
      case 2:
         shader = glCreateShader(GL_GEOMETRY_SHADER);
         shaderType = "GEOMETRY_SHADER";
         break;
   }
   
   glShaderSource(shader, 1, &source, nullptr);
   glCompileShader(shader);

   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   if (!success){
      glGetShaderInfoLog(shader, 512, nullptr, infoLog);
      std::cout << "ERROR::SHADER::"<< shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
   }

   return shader;
}

void Shader::use(){
   glUseProgram(shaderProgram);
}
void Shader::setBool(const std::string &name, bool val) const{
   glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int) val);
}
void Shader::setInt(const std::string &name, int val) const{
   glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), val);
}
void Shader::setFloat(const std::string &name, float val) const{
   glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), val);
}
void Shader::setVec2(const std::string &name, glm::vec2 val) const{
   glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), val.x, val.y);
}
void Shader::setVec3(const std::string &name, glm::vec3 val) const{
   glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), val.x, val.y, val.z);
}
void Shader::setVec4(const std::string &name, glm::vec4 val) const{
   glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), val.x, val.y, val.z, val.w);
}
void Shader::setMat4(const std::string &name, glm::mat4 val) const{
   glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}