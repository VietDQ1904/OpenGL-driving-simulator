#include "terrainPath.hpp"

float roundToDecimal(float value, int decimals) {
   float factor = std::pow(10.0, decimals);
   return std::round(value * factor) / factor;
}

float pointToSegmentDistance(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b) {
   glm::vec3 ab = b - a;
   glm::vec3 ap = p - a;
   float t = glm::dot(ap, ab) / glm::dot(ab, ab);
   t = glm::clamp(t, 0.0f, 1.0f);
   glm::vec3 projection = a + t * ab;
   return glm::length(p - projection);
}

float getNoiseMultiplierByDistance(float referenceDistance, float distance){
   float value = glm::clamp(distance / referenceDistance - 1.0f, 0.0f, 1.0f);
   value = roundToDecimal(std::pow(value, 5), 1) * 2.0f;
   return value;
}

void Terrain::generateIndices(){
   int topLeft, bottomLeft, topRight, bottomRight;

   for (int i = 0; i < vertices.size() / 3; i += 4){
      // First triangle
      indices.push_back(i);
      indices.push_back(i + 1);
      indices.push_back(i + 2);

      // Second triangle
      indices.push_back(i + 1);
      indices.push_back(i + 2);
      indices.push_back(i + 3);

   }

}

void Terrain::subdivide(std::vector<Triangle> &triangles, const Triangle &triangle, int depth){
   if (depth == 0){
      triangles.push_back(triangle);
      return;
   }  

   // Find the mid point of every edge.
   glm::vec3 ab = (triangle.a + triangle.b) * 0.5f;
   glm::vec3 bc = (triangle.b + triangle.c) * 0.5f;
   glm::vec3 ac = (triangle.a + triangle.c) * 0.5f;

   // Subdivide for smaller triangles.
   subdivide(triangles, {triangle.a, ab, ac}, depth - 1);
   subdivide(triangles, {triangle.b, bc, ab}, depth - 1);
   subdivide(triangles, {triangle.c, ac, bc}, depth - 1);
   subdivide(triangles, {ab, bc, ac}, depth - 1);
}  

void Terrain::subdivide(std::vector<Quadrilateral> &quadrilaterals, const Quadrilateral &quadrilateral, int depth){
   if (depth == 0){
      quadrilaterals.push_back(quadrilateral);
      return;
   }

   glm::vec3 ab = (quadrilateral.a + quadrilateral.b) * 0.5f;
   glm::vec3 cd = (quadrilateral.c + quadrilateral.d) * 0.5f;  

   subdivide(quadrilaterals, {quadrilateral.a, ab, quadrilateral.c, cd}, depth - 1);
   subdivide(quadrilaterals, {ab, quadrilateral.b, cd, quadrilateral.d}, depth - 1);
}

void Terrain::generateVertices(Physics &simulation){
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA1, prevB1, prevA2, prevB2;

   glm::vec3 A1, B1, C1, D1; // the left side 
   glm::vec3 A2, B2, C2, D2; // the right side

   glm::vec3 normal;
   glm::vec3 normal1, normal2;

   float segmentLength;
   float uCoord, vCoord;
   float uvScale = 0.2f;

   // Setup for grass path next to the road
   for (int i = 0; i < generatedPath.size() - 1; ++i){

      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0){
         A1 = w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i];
         B1 = w * (roadPathWidth / 2) + generatedPath[i];
         A2 = -w * (roadPathWidth / 2) + generatedPath[i];
         B2 = -w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i];
      }
      else{
         A1 = prevA1;
         B1 = prevB1;
         A2 = prevA2;
         B2 = prevB2;
      }

      C1 = w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i + 1];
      D1 = w * (roadPathWidth / 2) + generatedPath[i + 1];
      C2 = -w * (roadPathWidth / 2) + generatedPath[i + 1];
      D2 = -w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i + 1];

      // Save point C1, D1 of the current iteration.
      prevA1 = C1;
      prevB1 = D1;
      prevA2 = C2;
      prevB2 = D2;

      normal1 = glm::normalize(glm::cross(C1 - A1, B1 - A1));
      normal2 = glm::normalize(glm::cross(C2 - A2, B2 - A2));

      segmentLength = glm::length(generatedPath[i + 1] - generatedPath[i]);
      
      // 1st vertex of the left side
      vertices.push_back(A1.x); 
      vertices.push_back(A1.y); 
      vertices.push_back(A1.z);
      vertices.push_back(normal1.x); 
      vertices.push_back(normal1.y); 
      vertices.push_back(normal1.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 2nd vertex of the left side 
      vertices.push_back(B1.x);
      vertices.push_back(B1.y);
      vertices.push_back(B1.z);
      vertices.push_back(normal1.x); 
      vertices.push_back(normal1.y); 
      vertices.push_back(normal1.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.0f);

      // 3rd vertex of the left side
      vertices.push_back(C1.x);
      vertices.push_back(C1.y);
      vertices.push_back(C1.z);
      vertices.push_back(normal1.x); 
      vertices.push_back(normal1.y); 
      vertices.push_back(normal1.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 4th vertex of the left side
      vertices.push_back(D1.x);
      vertices.push_back(D1.y);
      vertices.push_back(D1.z);
      vertices.push_back(normal1.x); 
      vertices.push_back(normal1.y); 
      vertices.push_back(normal1.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.0f);

      // 1st vertex of the right side
      vertices.push_back(A2.x); 
      vertices.push_back(A2.y); 
      vertices.push_back(A2.z);
      vertices.push_back(normal2.x); 
      vertices.push_back(normal2.y); 
      vertices.push_back(normal2.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 2nd vertex of the right side 
      vertices.push_back(B2.x);
      vertices.push_back(B2.y);
      vertices.push_back(B2.z);
      vertices.push_back(normal2.x); 
      vertices.push_back(normal2.y); 
      vertices.push_back(normal2.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.0f);

      // 3rd vertex of the right side
      vertices.push_back(C2.x);
      vertices.push_back(C2.y);
      vertices.push_back(C2.z);
      vertices.push_back(normal2.x); 
      vertices.push_back(normal2.y); 
      vertices.push_back(normal2.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 4th vertex of the right side
      vertices.push_back(D2.x);
      vertices.push_back(D2.y);
      vertices.push_back(D2.z);
      vertices.push_back(normal2.x); 
      vertices.push_back(normal2.y); 
      vertices.push_back(normal2.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.0f);

      // Create a rigid body.
      simulation.createRigidBody(A1, B1, C1, D1, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
      simulation.createRigidBody(A2, B2, C2, D2, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
   }

   // Set up terrain.
   int elements = 0;
   float noiseValue;
   float distanceToRoad;
   float multiplierValue;
   int lastIndex = 0;
   glm::vec3 pivot;

   std::vector<float> verticesSub;
   std::vector<float> verticesLowSub;

   for (int i = 0; i < generatedPath.size() - 1; ++i){

      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0){
         A1 = w * (roadPathWidth / 2 + terrainPathWidth) + generatedPath[i];
         B1 = w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i];
         A2 = -w * (roadPathWidth / 2 + terrainPathWidth) + generatedPath[i];
         B2 = -w * (roadPathWidth / 2) + generatedPath[i];
      }
      else{
         A1 = prevA1;
         B1 = prevB1;
         A2 = prevA2;
         B2 = prevB2;
      }

      C1 = w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i + 1];
      D1 = w * (roadPathWidth / 2 + terrainPathWidth) + generatedPath[i + 1];
      C2 = -w * (roadPathWidth / 2 + terrainPathWidth / horizontalTiles) + generatedPath[i + 1];
      D2 = -w * (roadPathWidth / 2 + terrainPathWidth) + generatedPath[i + 1];

      // Save point C1, D1 of the current iteration.
      prevA1 = C1;
      prevB1 = D1;
      prevA2 = C2;
      prevB2 = D2;

      segmentLength = glm::length(generatedPath[i + 1] - generatedPath[i]);

      std::vector<Quadrilateral> quadrilaterals;

      Quadrilateral q1 = {A1, B1, C1, D1};
      Quadrilateral q2 = {A2, B2, C2, D2};

      subdivide(quadrilaterals, q1, horizontalTiles / 2);
      subdivide(quadrilaterals, q2, horizontalTiles / 2);

      // Standard resolution terrain
      for (Quadrilateral &q: quadrilaterals){
         std::vector<Triangle> triangles;

         Triangle t1 = {q.a, q.b, q.c};
         Triangle t2 = {q.b, q.c, q.d};
         subdivide(triangles, t1, subdivision);
         subdivide(triangles, t2, subdivision);

         for (Triangle &t : triangles) {

            for (glm::vec3 *p : {&t.a, &t.b, &t.c}){
               // Change Y coordinate of the point according to the noise values
               distanceToRoad = pointToSegmentDistance(*p, generatedPath[i], generatedPath[i + 1]);
               noiseValue = noise.getNoise(p->x * noiseScale, 0.0, p->z * noiseScale);
               multiplierValue = getNoiseMultiplierByDistance(terrainPathWidth / 2, distanceToRoad);
               p->y += multiplierValue * noiseValue * amplitude;
            }

            normal = glm::normalize(glm::cross(t.c - t.a, t.b - t.a));
         
            for (glm::vec3 p : {t.a, t.b, t.c}) {
               uCoord = glm::dot(p - generatedPath[i], v);
               vCoord = glm::dot(p - generatedPath[i], w);

               uCoord *= uvScale;
               vCoord *= uvScale;
    
               verticesSub.push_back(p.x); 
               verticesSub.push_back(p.y); 
               verticesSub.push_back(p.z);
               verticesSub.push_back(normal.x); 
               verticesSub.push_back(normal.y); 
               verticesSub.push_back(normal.z);
               verticesSub.push_back(uCoord); 
               verticesSub.push_back(vCoord);

            }
         }
      }

      // Generate a lower resolution terrain.
      for (Quadrilateral &q: quadrilaterals){
         std::vector<Triangle> triangles;

         Triangle t1 = {q.a, q.b, q.c};
         Triangle t2 = {q.b, q.c, q.d};
         subdivide(triangles, t1, 1);
         subdivide(triangles, t2, 1);

         for (Triangle &t : triangles) {

            for (glm::vec3 *p : {&t.a, &t.b, &t.c}){
               // Change Y coordinate of the point according to the noise values
               distanceToRoad = pointToSegmentDistance(*p, generatedPath[i], generatedPath[i + 1]);
               noiseValue = noise.getNoise(p->x * noiseScale, 0.0, p->z * noiseScale);
               multiplierValue = getNoiseMultiplierByDistance(terrainPathWidth / 2, distanceToRoad);
               p->y += multiplierValue * noiseValue * amplitude;
            }

            normal = glm::normalize(glm::cross(t.c - t.a, t.b - t.a));
         
            for (glm::vec3 p : {t.a, t.b, t.c}) {
               uCoord = glm::dot(p - generatedPath[i], v);
               vCoord = glm::dot(p - generatedPath[i], w);

               uCoord *= uvScale;
               vCoord *= uvScale;
    
               verticesLowSub.push_back(p.x); 
               verticesLowSub.push_back(p.y); 
               verticesLowSub.push_back(p.z);
               verticesLowSub.push_back(normal.x); 
               verticesLowSub.push_back(normal.y); 
               verticesLowSub.push_back(normal.z);
               verticesLowSub.push_back(uCoord); 
               verticesLowSub.push_back(vCoord);

            }
         }
      }
      
      if (elements >= partitionSize){
         verticesTerrain.push_back(verticesSub);
         verticesLowDetailsTerrain.push_back(verticesLowSub);

         pivot = generatedPath[lastIndex + (i - lastIndex) / 2]; 
         pivots.push_back(pivot);
         
         elements = 0;
         lastIndex = i;
         verticesSub.clear();
         verticesLowSub.clear();
      }

      elements++;
   }


   if (!verticesSub.empty()){
      pivot = generatedPath[lastIndex + (generatedPath.size() - 1 - lastIndex) / 2];
      pivots.push_back(pivot); 
      verticesTerrain.push_back(verticesSub);
   }

   if (!verticesLowSub.empty()){
      verticesLowDetailsTerrain.push_back(verticesLowSub);
   }


}

void Terrain::setUp(){

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

   glBindVertexArray(0);

   
   for (const auto& partition: verticesTerrain){
      GLuint vaoPartition, vboPartition;
      glGenVertexArrays(1, &vaoPartition);
      glGenBuffers(1, &vboPartition);

      vaos.push_back(vaoPartition);
      vbos.push_back(vboPartition);

      glBindVertexArray(vaoPartition);
      glBindBuffer(GL_ARRAY_BUFFER, vboPartition);
      glBufferData(GL_ARRAY_BUFFER, partition.size() * sizeof(float), partition.data(), GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

      glBindVertexArray(0);
   }

   for (const auto& partition: verticesLowDetailsTerrain){
      GLuint vaoPartition, vboPartition;
      glGenVertexArrays(1, &vaoPartition);
      glGenBuffers(1, &vboPartition);

      lowVaos.push_back(vaoPartition);
      lowVbos.push_back(vboPartition);

      glBindVertexArray(vaoPartition);
      glBindBuffer(GL_ARRAY_BUFFER, vboPartition);
      glBufferData(GL_ARRAY_BUFFER, partition.size() * sizeof(float), partition.data(), GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

      glBindVertexArray(0);
   }

}

void Terrain::render(Shader &shader, Camera &camera){

   model = glm::mat4(1.0f);
   shader.setMat4("model", model);

   glBindVertexArray(vao);
   glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);

   float length;
   for (int i = 0; i < vaos.size(); ++i){
      length = glm::distance(camera.cameraPos, pivots[i]);

      if (length < maxRenderDistance){
         if (length < renderDistance){
            glBindVertexArray(vaos[i]);
            // Use drawArrays instead because all triangles were already defined.
            glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(verticesTerrain[i].size() / 8));
         }
         
         glBindVertexArray(lowVaos[i]);
         glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(verticesLowDetailsTerrain[i].size() / 8));
      
      }

   }


   glBindVertexArray(0);
}

void Terrain::cleanUpBuffers(){
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &ebo);

   for (GLuint v: vaos){
      glDeleteVertexArrays(1, &v);
   }

   for (GLuint v: vbos){
      glDeleteBuffers(1, &v);
   }

   for (GLuint v: lowVaos){
      glDeleteVertexArrays(1, &v);
   }

   for (GLuint v: lowVbos){
      glDeleteVertexArrays(1, &v);
   }
}


