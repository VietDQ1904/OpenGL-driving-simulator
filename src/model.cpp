#include "shader.hpp"
#include "model.hpp"
// #include "loadTexture.hpp"
#include <string>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures){
   this->vertices = vertices;
   this->indices = indices;
   this->textures = textures;

   setUpMesh();
}

void Mesh::cleanUpBuffers(){
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &ebo);
}

void Mesh::draw(Shader &shader){
   glBindVertexArray(this->vao);
   glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);
}   

void Mesh::bindTextures(Shader &shader){
   unsigned int diffuseNr = 1;
   unsigned int specularNr = 1;
   unsigned int normalNr = 1;
   unsigned int metallicNr = 1;

   for (unsigned int i = 0; i < textures.size(); ++i){
      glActiveTexture(GL_TEXTURE0 + i);
      std::string number;
      std::string name = textures[i].type;

      if (name == "texture_diffuse"){
         number = std::to_string(diffuseNr++);
      }
      else if (name == "texture_roughness"){
         number = std::to_string(specularNr++);
      }
      else if (name == "texture_normal"){
         number = std::to_string(normalNr++);
      }
      else if (name == "texture_metallic"){
         number = std::to_string(metallicNr++);
      }
      shader.setInt((name + number).c_str(), i);
      textures[i].bindTexture();
   }

   glActiveTexture(GL_TEXTURE0);
}

 
void Mesh::setUpMesh(){
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
   
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

   glEnableVertexAttribArray(1);
   // offsetof return the byte offset of the attribute.
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));

   glEnableVertexAttribArray(3);
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));

   glEnableVertexAttribArray(4);
   glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

   glBindVertexArray(0);

}

Model::Model(const char *path){
   loadModel(path);
}

void Model::loadShader(std::string shaderName, const char* vertexFile, const char* fragmentFile, const char* geometryFile){
   modelShader = ResourceManagement::loadShader(shaderName, vertexFile, fragmentFile, geometryFile);
   this->shaderName = shaderName;
}


void Model::draw(){
   for (unsigned int i = 0; i < meshes.size(); ++i){
      meshes[i].bindTextures(modelShader);
      meshes[i].draw(modelShader);
   }
}

void Model::cleanUpBuffers(){
   for (unsigned int i = 0; i < meshes.size(); ++i){
      meshes[i].cleanUpBuffers();
   }
}


void Model::loadModel(std::string path){
   Assimp::Importer importer;
   const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
      std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
      return;
   }

   // Get the last directory of the path
   directory = path.substr(0, path.find_last_of('/'));
   // Process the root node.
   processNode(scene->mRootNode, scene);
}
      
void Model::processNode(aiNode *node, const aiScene *scene) {
   for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
      unsigned int meshIndex = node->mMeshes[i];
      aiMesh* mesh = scene->mMeshes[meshIndex];
      meshes.push_back(processMesh(mesh, scene));
   }

   // Recursively process child nodes.
   for (unsigned int i = 0; i < node->mNumChildren; ++i) {
      processNode(node->mChildren[i], scene);
   }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;
   std::vector<Texture> textures;

   for (unsigned int i = 0; i < mesh->mNumVertices; ++i){
      Vertex vertex;
      glm::vec3 vect;
      vect.x = mesh->mVertices[i].x;
      vect.y = mesh->mVertices[i].y;
      vect.z = mesh->mVertices[i].z;
      vertex.position = vect;
      
      vect.x = mesh->mNormals[i].x;
      vect.y = mesh->mNormals[i].y;
      vect.z = mesh->mNormals[i].z;
      vertex.normal = vect;
      
      if (mesh->mTextureCoords[0]){
         glm::vec2 vec;
         vec.x = mesh->mTextureCoords[0][i].x;
         vec.y = mesh->mTextureCoords[0][i].y;
         vertex.texCoords = vec;
      }
      else{
         vertex.texCoords = glm::vec2(0.0f, 0.0f);
      }

      vect.x = mesh->mTangents[i].x;
      vect.y = mesh->mTangents[i].y;
      vect.z = mesh->mTangents[i].z;
      vertex.tangent = vect;

      vect.x = mesh->mBitangents[i].x;
      vect.y = mesh->mBitangents[i].y;
      vect.z = mesh->mBitangents[i].z;
      vertex.bitangent = vect;

      vertices.push_back(vertex);
   }

   for (unsigned int i = 0; i < mesh->mNumFaces; ++i){
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j){
         indices.push_back(face.mIndices[j]);
      }
   }
   
   if (mesh->mMaterialIndex >= 0){ 
      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
      std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_roughness");
      textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
      std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
      std::vector<Texture> metalnessMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "texture_metallic");
      textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());
   }

   return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName){
   std::vector<Texture> textures;
   for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i){
      aiString str;
      mat->GetTexture(type, i, &str);

      std::string textureFile = directory + "/" + str.C_Str();
      std::string textureName = typeName + "_" + str.C_Str();
      
      Texture texture = ResourceManagement::loadTexture(textureName, textureFile.c_str(), true, false);
      texture.type = typeName;
      textures.push_back(texture);
   }

   return textures;
}

void Model::setDiffuseTexture(unsigned int materialIndex, const std::string &texturePath){

   Texture texture = ResourceManagement::loadTexture("texture_diffuse_" + texturePath, texturePath.c_str(), true, false);
   texture.type = "texture_diffuse";

   if (materialIndex >= meshes.size()){
      std::cout << "SET_DIFFUSE_TEXTURE::INVALID_MATERIAL_INDEX" << std::endl;
      return;
   }

   for (unsigned int i = 0; i < meshes[materialIndex].textures.size(); ++i){
      if (meshes[materialIndex].textures[i].type == "texture_diffuse"){
         meshes[materialIndex].textures[i] = texture;
      }
   }
}