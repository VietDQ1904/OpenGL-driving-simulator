#include "physics.hpp"


Physics::Physics(){
   this->collisionConfig = new btDefaultCollisionConfiguration;
   this->collisionDispatcher = new btCollisionDispatcher(this->collisionConfig);
   this->broadphaseInterface = new btDbvtBroadphase();
   this->solver = new btSequentialImpulseConstraintSolver();
   this->dynamicsWorld = new btDiscreteDynamicsWorld(this->collisionDispatcher, this->broadphaseInterface,
                                                     this->solver, this->collisionConfig);

   this->dynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

btRigidBody* Physics::createRigidBody(int type, glm::vec3 pos, glm::vec3 size, glm::vec3 rot, float m,
                                      float friction, float restitution, short group, short mask){

   btCollisionShape* collisionShape = nullptr;

   btVector3 position = btVector3(pos.x, pos.y, pos.z);
   
   btQuaternion rotation;
   rotation.setEuler(rot.x, rot.y, rot.z);
   btVector3 dimension;

   switch (type){
      case BOX:
         dimension = btVector3(size.x, size.y, size.z);
         collisionShape = new btBoxShape(dimension);
         break;
         
      case SPHERE:
         collisionShape = new btSphereShape(size.x);
         break;
   
      case CYLINDER:
         dimension = btVector3(size.x, size.y, size.z);
         collisionShape = new btCylinderShape(dimension);
         break;
   }

   this->collisionShapes.push_back(collisionShape);
   
   // Transform object.
   btTransform objTransform;
   objTransform.setIdentity();
   objTransform.setRotation(rotation);
   objTransform.setOrigin(position);


   btScalar mass = m;
   bool isDynamic = (mass != 0.0f); // Check if it's a static (not affected by forces) object

   // Calculate local inertia.
   btVector3 localInertia(0.0f, 0.0f, 0.0f);
   if (isDynamic){
      collisionShape->calculateLocalInertia(mass, localInertia);
   }
   
   btDefaultMotionState* motionState = new btDefaultMotionState(objTransform);

   // Calculate rigid body info.
   btRigidBody::btRigidBodyConstructionInfo rigidBodyInfo(mass, motionState, collisionShape, localInertia);

   rigidBodyInfo.m_friction = friction;
   rigidBodyInfo.m_restitution = restitution;

   if (type == SPHERE){
      rigidBodyInfo.m_angularDamping = 0.3f;
      rigidBodyInfo.m_rollingFriction = 0.3f;
   }
   else if (type == CYLINDER){
      rigidBodyInfo.m_angularDamping = 0.25f;
      rigidBodyInfo.m_rollingFriction = 0.75f;
   }

   btRigidBody* rigidBody = new btRigidBody(rigidBodyInfo);
   this->dynamicsWorld->addRigidBody(rigidBody, group, mask);

   return rigidBody;
}

void Physics::clear(){

   // Clean up rigid bodies.
   for (int i = this->dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i){
      btCollisionObject *obj = this->dynamicsWorld->getCollisionObjectArray()[i];
      btRigidBody *body = btRigidBody::upcast(obj);
      if (body && body->getMotionState()){
         delete body->getMotionState();
      }
      this->dynamicsWorld->removeCollisionObject(obj);
      delete obj;
   }
   
   // Clean up collision shapes
   for (int i = 0; i < this->collisionShapes.size(); ++i){
      btCollisionShape *shape = this->collisionShapes[i];
      this->collisionShapes[i] = 0;
      delete shape;
   }

   // Delete initialized objects.
   delete this->dynamicsWorld;
   delete this->solver;
   delete this->broadphaseInterface;
   delete this->collisionDispatcher;
   delete this->collisionConfig;

   this->collisionShapes.clear();
}