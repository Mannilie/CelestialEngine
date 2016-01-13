/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Ragdoll.h
@date: 24/07/2015
@author: Emmanuel Vaccaro
@brief: Handles ragdoll simulation using
the PhysXEngine
===============================================*/

#ifndef _RAGDOLL_H_
#define _RAGDOLL_H_

// Sub-engines
#include "PhysXEngine.h"

// Physics
#include "PhysicsObject.h"

// Parts which make up our ragdoll
enum RagDollParts {
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

// Forward declaration
class FBXSkeleton;

class Ragdoll : public PhysicsObject {
public:
	Ragdoll();
	virtual ~Ragdoll();
	bool Startup();
	void Shutdown();
	bool Update();

	FBXSkeleton* skeleton;
	RagdollNode** ragdollData;
};

#endif // _RAGDOLL_H_