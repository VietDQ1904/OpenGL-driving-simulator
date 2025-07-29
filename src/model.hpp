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
#include "shader.hpp"
#include "texture.hpp"

#ifndef MODEL_3D
#define MODEL_3D

struct Vertex{
   glm::vec3 position;
   glm::vec3 normal;
   glm::vec2 texCoords;
};

class Model;

class Mesh{
   public:
      std::vector<Vertex> vertices;
      std::vector<unsigned int> indices;
      std::vector<Texture> textures;
      unsigned int vao, vbo, ebo;

      Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);  
      void draw(Shader &shader);
      void cleanUpBuffers();
   
   private:
      void setUpMesh();

};

class Model{
   public:
      Model(const char *path);
      void draw(Shader &shader);
      void cleanUpBuffers();
      std::vector<Mesh> meshes;
      std::string directory;

   private:
      void loadModel(std::string path);
      void processNode(aiNode *node, const aiScene *scene);
      Mesh processMesh(aiMesh *mesh, const aiScene *scene);
      std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};


#endif