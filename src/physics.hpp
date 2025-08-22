#include <bullet/btBulletDynamicsCommon.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#ifndef PHYSICS_IMPLEMENTATION
#define PHYSICS_IMPLEMENTATION

enum CollisionShapes
{
   BOX,
   SPHERE,
   CYLINDER
};
enum CollisionFlags
{
   COLLISION_TERRAIN = 1,
   COLLISION_CHASSIS = 1 << 1,
   COLLISION_WHEEL = 1 << 2,
   COLLISION_CAR = COLLISION_CHASSIS | COLLISION_WHEEL,
   COLLISION_ELSE = -1
};

class Physics
{
public:
   btDiscreteDynamicsWorld *dynamicsWorld;
   btAlignedObjectArray<btCollisionShape *> collisionShapes;
   btDefaultCollisionConfiguration *collisionConfig;
   btCollisionDispatcher *collisionDispatcher;
   btBroadphaseInterface *broadphaseInterface;
   btSequentialImpulseConstraintSolver *solver;

   Physics();
   ~Physics();
   btRigidBody *createRigidBody(int type, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float m,
                                float friction, float restitution, short group, short mask);
   btRigidBody *createRigidBody(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D, float m,
                                float friction, float restitution, short group, short mask);
};

#endif