#ifndef PhysicsWorld_h
#define PhysicsWorld_h

#include <bullet/btBulletDynamicsCommon.h>
#include "world.h"
#include <vector>
#include <map>

using namespace std;

class Physics {
public:
	Physics() {
		world = createPhysicsWorld(GRAVITY);
	}
	bool simulate() {
		world->stepSimulation(1 / 60.f, 10);
		return true;
	}
	bool addSide(string name, glm::vec3 norm, glm::vec3 pos, GLfloat coe = COE) {
		rigidBodies[name] = makeSide(world, norm, pos, coe);
		return true;
	}
    bool addSphere(string name, btScalar mass, GLfloat r, glm::vec3 pos, glm::vec3 velocity = glm::vec3(), GLfloat coe = COE) {
		rigidBodies[name] = makeSphere(world, mass, r, pos, velocity, coe);
		return true;
	}
	btDiscreteDynamicsWorld * getWorld() {
		return world;
	}
	map<string, btRigidBody *> getAllRigidbodies() {
		return rigidBodies;
	}
	btRigidBody * getRigidBody(string name) {
		return rigidBodies[name];
	}
	vector<glm::vec3> getRigidBodyPositions(vector<string> names) {
		vector<glm::vec3> positions;
		for (GLuint i = 0; i < names.size(); i++) {
			positions.push_back(getRigidBodyPosition(names[i]));
		}
		return positions;
	}
	glm::vec3 getRigidBodyPosition(string name) {
		getRigidBody(name)->getMotionState()->getWorldTransform(trans);
		return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	}
private:
	btDiscreteDynamicsWorld * world;
	map<string, btRigidBody *> rigidBodies;
	btTransform trans;

	btDiscreteDynamicsWorld * createPhysicsWorld(GLfloat gravity = GRAVITY) {

		// create the world parameters
		btBroadphaseInterface * broadphase = new btDbvtBroadphase();
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld * dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0., gravity, 0));

        return dynamicsWorld;
	}

	btRigidBody* makeSide(btDiscreteDynamicsWorld* dynamicsWorld, glm::vec3 norm, glm::vec3 pos, GLfloat coe) {

		btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(norm.x, norm.y, norm.z), 0); // declare a plane shape at the right orientation
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z))); // no rotation, located at the right place
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0)); // the rigid body parameters
		btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI); // define the rigid body

		groundRigidBody->setRestitution(coe);
		dynamicsWorld->addRigidBody(groundRigidBody); // add to the world

		return groundRigidBody;
	}

    btRigidBody* makeSphere(btDiscreteDynamicsWorld* dynamicsWorld, GLfloat mass, GLfloat r, glm::vec3 pos, glm::vec3 velocity, GLfloat coe) {

		btCollisionShape* fallShape = new btSphereShape(r); // create a r radius sphere shape
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z))); // position it at right place, no rotation
		btVector3 fallInertia(0, 0, 0);
		fallShape->calculateLocalInertia(mass, fallInertia); // calculate the inertia
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia); // rigid body params
		btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI); // create the rigid body

		fallRigidBody->setRestitution(coe);
		fallRigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
		dynamicsWorld->addRigidBody(fallRigidBody);

		return fallRigidBody;

	}
};

#endif /* PhysicsWorld_h */
