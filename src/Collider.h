/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Collider.h
@date: 24/07/2015
@author: Emmanuel Vaccaro
@brief: A base class for all colliders.
===============================================*/

#ifndef _COLLIDER_H_
#define _COLLIDER_H_

// Sub-engines
#include "PhysicsEngine.h"

// Components
#include "Component.h"

// Structs
#include "Bounds.h"
#include "Ray.h"

class Rigidbody;
class Collider : public Component {
public:
	Collider() : attachedRigidbody(nullptr), enabled(true) {}
	~Collider() {}
	virtual bool Startup() { return true; }
	virtual void Shutdown() {}
	virtual bool Update() { return true; }
	virtual void Draw() {}
	virtual bool Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance) { return false; }
	
	Bounds bounds;
	bool enabled; // Enabled Colliders will collide with other colliders
	bool isTrigger; // Is the collider a trigger?
	float contactOffset; // Contact offset value of this collider.
	Rigidbody* attachedRigidbody; // The rigidbody the collider is attached to. Returns null if the collider is attached to no rigidbody.
	ShapeID shapeId;
};

#endif // _COLLIDER_H_