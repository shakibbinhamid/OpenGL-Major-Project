#ifndef PhysicsWorld_h
#define PhysicsWorld_h

#include <bullet/btBulletDynamicsCommon.h>
#include "world.h"
#include <vector>

using namespace std;

class Physics {
public:
	Physics() {
		this->createPhysicsWorld(world, GRAVITY);
	}
	bool addSide(glm::vec3 norm, glm::vec3 pos, GLfloat coe = COE) {
		walls.push_back(makeSide(world, norm, pos, coe));
		return true;
	}
	bool addSphere(btScalar mass, GLfloat r, glm::vec3 pos, GLfloat coe = COE) {
		rigidSpheres.push_back(makeSphere(world, mass, r, pos, coe));
		return true;
	}
	bool simulate() {
		world->stepSimulation(1 / 60.f, 10);
		return true;
	}
	btDiscreteDynamicsWorld * getWorld() {
		return world;
	}
	vector<btRigidBody *> getSpheres() {
		return rigidSpheres;
	}
	vector<glm::vec3> getSpherePositions() {
		vector<glm::vec3> positions;
		btTransform trans;
		for (GLuint i = 0; i < rigidSpheres.size(); i++) {
			rigidSpheres[i]->getMotionState()->getWorldTransform(trans);
			positions.push_back(glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
		}
		return positions;
	}
private:
	btDiscreteDynamicsWorld * world;
	vector<btRigidBody *> walls;
	vector<btRigidBody *> rigidSpheres;

	void createPhysicsWorld(btDiscreteDynamicsWorld* dynamicsWorld, GLfloat gravity = GRAVITY) {

		// create the world parameters
		btBroadphaseInterface * broadphase = new btDbvtBroadphase();
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0., gravity, 0));

	}

	btRigidBody* makeSide(btDiscreteDynamicsWorld* dynamicsWorld, glm::vec3 norm, glm::vec3 pos, GLfloat coe = COE) {

		btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(norm.x, norm.y, norm.z), 0); // declare a plane shape at the right orientation
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z))); // no rotation, located at the right place
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0)); // the rigid body parameters
		btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI); // define the rigid body

		groundRigidBody->setRestitution(coe);
		dynamicsWorld->addRigidBody(groundRigidBody); // add to the world

		return groundRigidBody;
	}

	btRigidBody* makeSphere(btDiscreteDynamicsWorld* dynamicsWorld, GLfloat mass, GLfloat r, glm::vec3 pos, GLfloat coe = COE) {

		btCollisionShape* fallShape = new btSphereShape(r); // create a r radius sphere shape
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z))); // position it at right place, no rotation
		btVector3 fallInertia(0, 0, 0);
		fallShape->calculateLocalInertia(mass, fallInertia); // calculate the inertia
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia); // rigid body params
		btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI); // create the rigid body

		fallRigidBody->setRestitution(coe);
		fallRigidBody->setLinearVelocity(btVector3(50, 0, 0));
		dynamicsWorld->addRigidBody(fallRigidBody);

		return fallRigidBody;

	}
};

#endif /* PhysicsWorld_h */
