/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: CustpmPhysicsEngine.h
@date: 24/06/2015
@author: Emmanuel Vaccaro
@brief: A physics engine class that is able
to be modified to create one's own physics 
engine capabilities.
===============================================*/

#ifndef _CUSTOM_PHYSICS_ENGINE_H_
#define _CUSTOM_PHYSICS_ENGINE_H_

// Sub-engines
#include "PhysicsEngine.h"

struct CollisionManifold {
	bool isColliding;

	vec3 point;
	vec3 normal;
	float restitution;

	float intersectionA;
	float intersectionB;

	Collider* colliderA;
	Collider* colliderB;
};

class Rigidbody;
class CustomPhysicsEngine : public PhysicsEngine {
public:
	CustomPhysicsEngine(){}
	virtual ~CustomPhysicsEngine(){}
	void Shutdown();
	bool Update();
	void AddActor(PhysicsObject* _actor);
	bool RemoveActor(PhysicsObject* _actor);
	void AddArticulation(PhysicsObject* _articulation){}
	bool RemoveArticulation(PhysicsObject* _articulation){}
	void CheckForCollisions();
	void ApplyCollisionResolution(CollisionManifold* _manifold, Rigidbody* _rigid);
	void ApplyCollisionResolutionTest(CollisionManifold* _manifold, Rigidbody* _rigidA, Rigidbody* _rigidB);
	static CollisionManifold SphereToSphere(Collider* _first, Collider* _second);
	static CollisionManifold SphereToPlane(Collider* _sphere, Collider* _plane);
	static CollisionManifold PlaneToSphere(Collider* _plane, Collider* _sphere);
	static CollisionManifold BoxToBox(Collider* _first, Collider* _second);
	static CollisionManifold BoxToPlane(Collider* _box, Collider* _plane);
	static CollisionManifold PlaneToBox(Collider* _plane, Collider* _box);
	static CollisionManifold SphereToBox(Collider* _sphere, Collider* _box);
	static CollisionManifold BoxToSphere(Collider* _box, Collider* _sphere);
	
	vector<PhysicsObject*> actors;
	vector<PhysicsObject*> articulations;
};

#endif // _CUSTOM_PHYSICS_ENGINE_H_