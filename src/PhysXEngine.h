/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: PhysXEngine.h
@date: 24/06/2015
@author: Emmanuel Vaccaro
@brief: Handles all physics interactions with 
GameObjects
===============================================*/

#ifndef _PHYSX_ENGINE_H_
#define _PHYSX_ENGINE_H_

// Sub-engines
#include "PhysicsEngine.h"

// Components
#include "Collider.h"

// Other
#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
using namespace physx;

struct PhysXActor {
	ShapeID shapeId;
	PhysicsObject* physicsObject;
	PxActor* pxActor;
	PhysXActor(PhysicsObject* _physicsObject) :
		physicsObject(_physicsObject),
		pxActor(nullptr){}
};

struct PhysXArticulation {
	ShapeID shapeId;
	PhysicsObject* physicsObject;
	PxArticulation* pxArticulation;
	PhysXArticulation(PhysicsObject* _physicsObject) :
		physicsObject(_physicsObject),
		pxArticulation(nullptr){}
};

// Create some constants for axis of rotation to make
// definition of quaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);
struct RagdollNode {
	PxQuat globalRotation;
	PxVec3 scaledGlobalPosition;

	int parentNodeIndex;
	float halfLength;
	float radius;
	float parentLinkPosition;

	float childLinkPosition;
	char* name;

	PxArticulationLink* linkPointer;

	RagdollNode(PxQuat _globalRotation, int _parentNodeIndex, float _halfLength, float
		_radius, float _parentLinkPosition, float _childLinkPosition, char* _name) :
		globalRotation(_globalRotation), parentNodeIndex(_parentNodeIndex),
		halfLength(_halfLength), radius(_radius), parentLinkPosition(_parentLinkPosition),
		childLinkPosition(_childLinkPosition), name(_name) {}
};

class PhysXControllerHitReportCallback;
class ParticleEmitter;
class Rigidbody;
class CharacterController;
class Ragdoll;

class PhysXEngine : public PhysicsEngine {
public:
	PhysXEngine(){}
	virtual ~PhysXEngine(){}
	bool Startup();
	void Shutdown();
	bool Update();
	void AddActor(PhysicsObject* _actor);
	bool RemoveActor(PhysicsObject* _actor);
	void AddArticulation(PhysicsObject* _articulation);
	bool RemoveArticulation(PhysicsObject* _articulation);
	void SetupVisualDebugger();
	PxGeometry* GetColliderGeometry(Collider* _collider);
	vector<PxGeometry*> GetMeshColliderGeometry(Collider* _collider);
	PxGeometry* CreateShape(ShapeID _shapeId);
	//Actors
	bool CreateParticleEmitter(ParticleEmitter& _particleEmitter, PhysicsObject* _actor);
	bool CreateRigidbody(Rigidbody& _particleEmitter, PhysicsObject* _actor);
	bool CreateCharacterController(CharacterController& _characterController, PhysicsObject* _actor);
	//Articulations
	bool CreateRagdoll(Ragdoll& _ragdoll, PhysicsObject* _articulation);
	bool UpdateActorTransform(PhysXActor* _actor);
	//Actors
	bool UpdateActorParticleEmitter(PhysXActor* _actor);
	bool UpdateActorRigidbody(PhysXActor* _actor);
	bool UpdateActorCharacterController(PhysXActor* _actor);
	//Articulations
	bool UpdateArticulationRagdoll(PhysXArticulation* _articulation);

	vector<PhysXActor> actors;
	vector<PhysXArticulation> articulations;
	float characterYVelocity;
	PxDefaultErrorCallback defaultErrorCallback;
	PxDefaultAllocator defaultAllocator;
	PxSimulationFilterShader defaultFilterShader;
	vector<PxArticulation*>	ragdollActorArticulations;
	PxFoundation* physicsFoundation;
	PxPhysics* physics;
	PxScene* physicsScene;
	PxMaterial* physicsMaterial;
	PxMaterial*	boxMaterial;
	PxMaterial*	ragdollMaterial;
	PxCooking* physicsCooker;
	PxProfileZoneManager* profileZoneManager;
	PxCudaContextManager* cudaContextManager;
	PxControllerManager* characterManager;
	PhysXControllerHitReportCallback* characterHitReport;
	PxMaterial*	playerPhysicsMaterial;
	PxController* playerController;
};


#endif //_PHYSX_ENGINE_H_