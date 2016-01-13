/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: PhysicsEngine.h
@date: 24/06/2015
@author: Emmanuel Vaccaro
@brief: A base class for all physics engines
that get switched out in the game engine.
===============================================*/

#ifndef _PHYSICS_ENGINE_H_
#define _PHYSICS_ENGINE_H_

#include "GLM_Header.h"

#include <vector>
using std::vector;

enum ShapeID
{
	SHAPE_PLANE = 0,
	SHAPE_SPHERE = 1,
	SHAPE_BOX = 2,
	SHAPE_CAPSULE = 3, 
	SHAPE_MESH = 4,
	SHAPE_COUNT = 3
};

class PhysicsObject;
class Collider;

class PhysicsEngine
{
public:
	PhysicsEngine() : collisionEnabled(true), timeStep(0.001f), gravity(vec3(0, -9.807f, 0)) {}
	~PhysicsEngine(){}

	virtual bool Startup(){ return true; }
	virtual void Shutdown(){}

	virtual bool Update() = 0;
	
	virtual void AddActor(PhysicsObject* _actor) = 0;
	virtual bool RemoveActor(PhysicsObject* _actor) = 0;

	virtual void AddArticulation(PhysicsObject* _articulation) = 0;
	virtual bool RemoveArticulation(PhysicsObject* _articulation) = 0;

	bool collisionEnabled;
	float timeStep;
	vec3 gravity;
};

#endif //_PHYSICS_ENGINE_H_