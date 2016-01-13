/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Rigidbody.h
@date: 14/07/2015
@author: Emmanuel Vaccaro
@brief: Places motion of GameObject under 
control of the Physics engine
===============================================*/

#ifndef _RIGID_BODY_H_
#define _RIGID_BODY_H_

// Physics
#include "PhysicsObject.h"

// Utilities
#include "GLM_Header.h"

class Rigidbody : public PhysicsObject {
public:
	Rigidbody();
	~Rigidbody();
	bool Startup(); 
	void Shutdown();
	bool Update();	
	virtual void PhysicsUpdate(float _timeStep);
	void AddForce(vec3 _force);
	void AddForceAtPosition(vec3 _force, vec3 _position);
	void AddTorque(vec3 _torque);
	void CalculateMomentOfInertia();
	bool HasChangedInGUI();
	void Inspector();

	quat inertiaTensorRotation; // If you don't set intertia tensor rotation from a script it will be calculated automatically from all colliders attached to the rigidbody.
	vec3 centerOfMass;
	vec3 worldCenterOfMass;
	vec3 inertiaTensor; // The diagonal inertia tensor of mass relative to the center of mass.
	vec3 totalTorque;
	vec3 totalForce;
	vec3 linearVelocity;
	vec3 angularVelocity; // In most cases you should not modify it directly, as this can result in unrealistic behaviour.
	vec3 angularMomentum;
	vec3 OldPosition;
	vec3 velocity;
	float momentOfInertia;
	float mass;
	float density;
	float staticFriction;
	float dynamicFriction;
	float maxAngularVelocity;
	float drag;
	float angularDrag; // Angular drag can be used to slow down the rotation of an object. The higher the drag the more the rotation slows down.
	bool useGravity;
	bool enabled;
	bool detectCollisions;
	bool isKinematic; // Controls whether physics affects the rigidbody.
	bool isChangedInGUI;
};

#endif // _RIGID_BODY_H_