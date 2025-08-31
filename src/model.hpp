#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "resourceManagement.hpp"

#ifndef MODEL_3D
#define MODEL_3D

struct Vertex
{
   glm::vec3 position;
   glm::vec3 normal;
   glm::vec2 texCoords;
   glm::vec3 tangent;
   glm::vec3 bitangent;
};

class Model;

class Mesh
{
public:
   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;
   std::vector<Texture> textures;
   unsigned int material;
   GLuint vao, vbo, ebo;

   Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int material);
   void draw(Shader &shader);
   void cleanUpBuffers();
   void bindTextures(Shader &shader);
   void setUpMesh();
};

class Model
{
public:
   std::vector<Mesh> meshes;
   std::string directory;
   Shader modelShader;
   std::string shaderName;

   Model(const char *path);
   void loadTextures();
   void loadShader(std::string shaderName, const char *vertexFile, const char *fragmentFile, const char *geometryFile);
   void draw();
   void cleanUpBuffers();
   void setDiffuseTexture(unsigned int materialIndex, const std::string &texturePath);

private:
   const aiScene *scene;
   Assimp::Importer importer;

   void loadModel(std::string path);
   void processNode(aiNode *node);
   Mesh processMesh(aiMesh *mesh, const aiScene *scene);
   std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif