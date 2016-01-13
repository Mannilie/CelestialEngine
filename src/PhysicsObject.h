/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: PhysicsObject.h
@date: 14/07/2015
@author: Emmanuel Vaccaro
@brief: Base class for physics simulated 
objects
===============================================*/

#ifndef _PHYSICS_OBJECT_H_
#define _PHYSICS_OBJECT_H_

// Components
#include "Component.h"

// Forward declaration
class RenderingEngine;

class PhysicsObject : public Component {
public:
	PhysicsObject(){}
	~PhysicsObject(){}
	virtual bool Startup(){ return true; }
	virtual void Shutdown(){}
	virtual bool Update(){ return true; }
	virtual void Draw(RenderingEngine& _renderer){}
	virtual void PhysicsUpdate(float _timeStep){}
};

#endif //_PHYSICS_OBJECT_H_