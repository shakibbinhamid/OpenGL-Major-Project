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
	bool addBox(string name, GLfloat x, GLfloat y, GLfloat z, glm::vec3 pos, GLfloat coe = COE) {
		rigidBodies[name] = makeBox(world, x, y, z, pos, coe);
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
	void shutdown() {
		for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--) {
			btCollisionObject* obj = world->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState()) {
				delete body->getMotionState();
			}
			world->removeCollisionObject(obj);
			delete obj;
		}
		//for (int j = 0; j <mCollisionShapes.size(); j++)
		//{
		//	btCollisionShape* shape = mCollisionShapes[j];
		//	delete shape;
		//}
		//mCollisionShapes.clear();
		delete world;
		delete solver;
		delete broadphase;
		delete dispatcher;
		delete collisionConfiguration;
	}
private:
	btDiscreteDynamicsWorld * world;
	map<string, btRigidBody *> rigidBodies;
	btTransform trans;

	btBroadphaseInterface * broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

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

	btRigidBody* makeBox(btDiscreteDynamicsWorld* dynamicsWorld, GLfloat x, GLfloat y, GLfloat z,  glm::vec3 pos, GLfloat coe){

		btBoxShape* boxShape = new btBoxShape(btVector3(x, y, z));
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		btDefaultMotionState* boxMotionState = new btDefaultMotionState(groundTransform);
		
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, boxMotionState, boxShape, btVector3(0, 0, 0)); // the rigid body parameters
		btRigidBody* boxRigidBody = new btRigidBody(groundRigidBodyCI); // define the rigid body

		boxRigidBody->setRestitution(coe);
		dynamicsWorld->addRigidBody(boxRigidBody);

		return boxRigidBody;
	}
};

#endif /* PhysicsWorld_h */
