/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: SphereCollider.h
@date: 25/07/2015
@author: Emmanuel Vaccaro
@brief: A sphere-shaped primitive collider.
===============================================*/

#ifndef _SPHERE_COLLIDER_H_
#define _SPHERE_COLLIDER_H_

// Components
#include "Collider.h"

class SphereCollider : public Collider {
public:
	SphereCollider();
	~SphereCollider();
	virtual bool Startup();
	virtual bool Update();
	void Inspector();

	vec3 center;
	float radius;
	float momentOfInertia;
};

#endif //_SPHERE_COLLIDER_H_