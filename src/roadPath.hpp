#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef ROAD_PATH
#define ROAD_PATH

class Road{
   public:
      std::vector<glm::vec3> points;
      float tension = 0.0f;
      float alpha = 0.5f;
      
      Road();


   private:
      glm::vec3 catmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
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
         float t_actual = 1;
         

      }

};


#endif