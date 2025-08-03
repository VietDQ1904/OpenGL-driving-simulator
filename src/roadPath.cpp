#include "roadPath.hpp"

void Road::generateIndices(){
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

void Road::generateVertices(Physics &simulation){
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA, prevB;

   glm::vec3 A;
   glm::vec3 B;
   glm::vec3 C;
   glm::vec3 D;

   glm::vec3 normal;

   float segmentLength;

   for (int i = 0; i < generatedPath.size() - 1; ++i){

      v = glm::normalize(generatedPath[i + 1] - generatedPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0){ // Only calculate A and B for the first iteration
         A = w * (roadPathWidth / 2) + generatedPath[i];
         B = -w * (roadPathWidth / 2) + generatedPath[i];
      }
      else{  // Assign the previous points of C and D.
         A = prevA;
         B = prevB;
      }

      C = w * (roadPathWidth / 2) + generatedPath[i + 1];
      D = -w * (roadPathWidth / 2) + generatedPath[i + 1];

      // Save point C, D of the current iteration.
      prevA = C;
      prevB = D;

      normal = glm::normalize(glm::cross(C - A, B - A));

      segmentLength = glm::length(generatedPath[i + 1] - generatedPath[i]);

      // 1st vertex
      vertices.push_back(A.x); 
      vertices.push_back(A.y); 
      vertices.push_back(A.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 2nd vertex 
      vertices.push_back(B.x);
      vertices.push_back(B.y);
      vertices.push_back(B.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.0f); 
      vertices.push_back(0.0f);

      // 3rd vertex
      vertices.push_back(C.x);
      vertices.push_back(C.y);
      vertices.push_back(C.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.2f * (roadPathWidth));

      // 4th vertex
      vertices.push_back(D.x);
      vertices.push_back(D.y);
      vertices.push_back(D.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.2f * (segmentLength)); 
      vertices.push_back(0.0f);

      // Create a rigid body.
      simulation.createRigidBody(A, B, C, D, normal, 0.0f, 0.5f, 0.5f, COLLISION_TERRAIN, COLLISION_ELSE);
   
   }

}

void Road::setUp(){
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
}

void Road::render(Shader &shader){

   model = glm::mat4(1.0f);
   shader.setMat4("model", model);

   glBindVertexArray(vao);
   glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
   glBindVertexArray(0);
}

void Road::cleanUpBuffers(){
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
   glDeleteBuffers(1, &ebo);
}
