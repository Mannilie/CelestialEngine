/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: CharacterController.h
@date: 02/07/2015
@author: Emmanuel Vaccaro
@brief: Character controller that player 
interacts with to view environment
===============================================*/

#ifndef _CHARACTER_CONTROLLER_H_
#define _CHARACTER_CONTROLLER_H_

// Utilities
#include "GLM_Header.h"

// Physics
#include "PhysicsObject.h"

class CharacterController : public PhysicsObject {
public:
	CharacterController();
	~CharacterController();
	bool Startup();
	void Shutdown();
	bool Update();
	void Draw(RenderingEngine& _renderer);
	void Inspector();

	float yRotation;
	vec3 velocity;
	float height;
	float radius;
	float gravity;
	float density;
	float slopeLimit;
	float stepOffset;
	float movementSpeed;
	bool useGravity;
	bool isGrounded;
	bool enabled;
	bool isChangedInGUI;
};

#endif // _CHARACTER_CONTROLLER_H_