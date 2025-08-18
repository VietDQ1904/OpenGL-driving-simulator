#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <vector>
#include <array>
#include <cmath>
#include <cstddef>  

#ifndef VERTICES_IMPLEMENTATION
#define VERTICES_IMPLEMENTATION

template<size_t N>
struct ArrayHash{
   std::size_t operator()(const std::array<float, N> &array) const{
      std::size_t h = 0;
      for (float val: array){
         std::size_t hv = std::hash<int>()(static_cast<int>(val * 1000.0f));
         h ^= hv + 0x9e3779b9 + (h << 6) + (h >> 2);
      }
      return h;
   }
};

template<size_t N>
struct ArrayEqual{
   bool operator()(const std::array<float, N> &a, const std::array<float, N> &b) const{
      const float epsilon = 1e-3f;
      for (size_t i = 0; i < N; ++i){
         if (fabs(a[i] - b[i]) > epsilon){
            return false;
         }
      }
      return true;
   }
};

class Vertices{

   public:
      float cellSize = 20.0f;

      std::unordered_map<std::array<float, 3>, std::vector<float>, ArrayHash<3>, ArrayEqual<3>> vertices;
      std::unordered_map<std::array<float, 3>, std::vector<float>, ArrayHash<3>, ArrayEqual<3>> lowDetailVertices;
      std::unordered_map<std::array<float, 2>, std::array<float, 3>, ArrayHash<2>, ArrayEqual<2>> gridMap;
      std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> vaos;
      std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> vbos;
      std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> vaosLD;
      std::unordered_map<std::array<float, 3>, GLuint, ArrayHash<3>> vbosLD;

      Vertices(){

      };

      void insertVertices(std::array<float, 3> const &key, std::vector<float> value){
         vertices[key] = value;
      }

      void insertLowDetailVertices(std::array<float, 3> const &key, std::vector<float> value){
         lowDetailVertices[key] = value;
      }

      void insertGridMap(std::array<float, 3> pivot){
         int cellX = static_cast<int>(std::floor(pivot[0] / cellSize));
         int cellZ = static_cast<int>(std::floor(pivot[2] / cellSize));
         std::array<float, 2> cellKey = {static_cast<float>(cellX), static_cast<float>(cellZ)};
         
         gridMap[cellKey] = pivot;
      }

      void insertVAO(std::array<float, 3> const &key, GLuint value){
         vaos[key] = value;
      }
      
      void insertVBO(std::array<float, 3> const &key, GLuint value){
         vbos[key] = value;
      }

      void insertLDVAO(std::array<float, 3> const &key, GLuint value){
         vaosLD[key] = value;
      }

      void insertLDVBO(std::array<float, 3> const &key, GLuint value){
         vbosLD[key] = value;
      }

      void clearBuffers(){
         for (auto i: vaos){
            glDeleteVertexArrays(1, &i.second);
         }
         
         for (auto i: vbos){
            glDeleteBuffers(1, &i.second);
         }

         for (auto i: vaosLD){
            glDeleteVertexArrays(1, &i.second);
         }

         for (auto i: vbosLD){
            glDeleteVertexArrays(1, &i.second);
         }
      }

      void findPivotsByRange(const std::array<float, 3> point, float distance, std::vector<std::array<float, 3>> &nearbyPoints){
         int pointCellX = static_cast<int>(std::floor(point[0] / cellSize));
         int pointCellZ = static_cast<int>(std::floor(point[2] / cellSize));
         
         float cellRadius = static_cast<int>(std::ceil(distance / cellSize));

         for (int dx = -cellRadius; dx <= cellRadius; ++dx){
            for (int dz = -cellRadius; dz <= cellRadius; ++dz){
               int nx = pointCellX + dx;
               int nz = pointCellZ + dz;
               
               std::array<float, 2> key = {static_cast<float> (nx), static_cast<float> (nz)};
               auto iter = gridMap.find(key);
               if (iter != gridMap.end()){
                  float posX = iter->second[0] - point[0];
                  float posZ = iter->second[2] - point[2];
                  float dist2 = posX * posX + posZ * posZ;

                  if (dist2 < distance * distance){
                     nearbyPoints.push_back(iter->second);
                  }
               }
            }
         }

      }

};

#endif