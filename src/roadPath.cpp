#include "roadPath.hpp"

glm::vec3 Road::catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                     const glm::vec3 &p2, const glm::vec3 &p3,
                     float t){

   auto tj = [](const glm::vec3& a, const glm::vec3& b, float alpha) {
      return std::pow(glm::length(b - a), alpha);
   };
      
   float t0 = 0.0f;
   float t1 = t0 + tj(p0, p1, alpha);
   float t2 = t1 + tj(p1, p2, alpha);
   float t3 = t2 + tj(p2, p3, alpha);
   
   // Change t from [0, 1] -> [t1, t2]
   float t_actual = t1 + (t2 - t1) * t;

   glm::vec3 A1 = ((t1 - t_actual) / (t1 - t0)) * p0 + ((t_actual - t0) / (t1 - t0)) * p1;
   glm::vec3 A2 = ((t2 - t_actual) / (t2 - t1)) * p1 + ((t_actual - t1) / (t2 - t1)) * p2;
   glm::vec3 A3 = ((t3 - t_actual) / (t3 - t2)) * p2 + ((t_actual - t2) / (t3 - t2)) * p3;

   glm::vec3 B1 = ((t2 - t_actual) / (t2 - t0)) * A1 + ((t_actual - t0) / (t2 - t0)) * A2;
   glm::vec3 B2 = ((t3 - t_actual) / (t3 - t1)) * A2 + ((t_actual - t1) / (t3 - t1)) * A3;

   glm::vec3 C  = ((t2 - t_actual) / (t2 - t1)) * B1 + ((t_actual - t1) / (t2 - t1)) * B2;
   
   return C;
}

void Road::generateSpline(){

   // Catmull-Rom spline must have at least 4 points.
   if (points.size() < 4){
      return;
   }

   // Generate Catmull-Rom spline curve.
   for (size_t i = 1; i < points.size() - 2; ++i){
      for (int j = 0; j < samplePerFragments; ++j){
         float t = static_cast<float>(j) / samplePerFragments;
         glm::vec3 point = catmullRom(points[i - 1],
                                      points[i],
                                      points[i + 1],
                                      points[i + 2], 
                                      t);
         
         roadPath.push_back(point);

      }
   }

}

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

void Road::generateVertices(){
   glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 v;
   glm::vec3 w;

   glm::vec3 prevA, prevB;

   glm::vec3 A;
   glm::vec3 B;
   glm::vec3 C;
   glm::vec3 D;

   glm::vec3 normal;

   float accumulatedLength = 0.0f;
   float segmentLength;
   float v0, v1;

   for (int i = 0; i < roadPath.size() - 1; ++i){

      v = glm::normalize(roadPath[i + 1] - roadPath[i]);
      w = glm::normalize(glm::cross(u, v));

      if (i == 0){ // Only calculate A and B for the first iteration
         A = w * (roadPathWidth / 2) + roadPath[i];
         B = -w * (roadPathWidth / 2) + roadPath[i];
      }
      else{  // Assign the previous points of C and D.
         A = prevA;
         B = prevB;
      }

      C = w * (roadPathWidth / 2) + roadPath[i + 1];
      D = -w * (roadPathWidth / 2) + roadPath[i + 1];

      // Save point C, D of the current iteration.
      prevA = C;
      prevB = D;

      normal = glm::normalize(glm::cross(C - A, B - A));

      segmentLength = glm::length(roadPath[i + 1] - roadPath[i]);
      v0 = accumulatedLength / tileLength;
      v1 = (accumulatedLength + segmentLength) / tileLength;
      
      vertices.push_back(A.x); 
      vertices.push_back(A.y); 
      vertices.push_back(A.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.0f); 
      vertices.push_back(v0);

      vertices.push_back(B.x);
      vertices.push_back(B.y);
      vertices.push_back(B.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(0.0f); 
      vertices.push_back(v1);

      vertices.push_back(C.x);
      vertices.push_back(C.y);
      vertices.push_back(C.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(1.0f); 
      vertices.push_back(v0);

      vertices.push_back(D.x);
      vertices.push_back(D.y);
      vertices.push_back(D.z);
      vertices.push_back(normal.x); 
      vertices.push_back(normal.y); 
      vertices.push_back(normal.z);
      vertices.push_back(1.0f); 
      vertices.push_back(v1);

      accumulatedLength += segmentLength;
   }

   // std::cout << "vertices = {";
   // for (int i = 0; i < vertices.size(); i += 8){
   //    std::cout << "(" << vertices[i] << "," << vertices[i + 1] << "," << vertices[i + 2] << ","
   //    << vertices[i + 3] << "," << vertices[i + 4] << "," << vertices[i + 5] << "," << 
   //    vertices[i + 6] << "," << vertices[i + 7] << "),";
   // }
   // std::cout << "}\n";
   // std::cout << "\n" << "Vertices size: " << vertices.size() << "\n";


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
   model = glm::translate(model, glm::vec3(1.0f, 3.0f, 1.0f));
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
