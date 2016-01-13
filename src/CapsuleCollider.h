/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: CapsuleCollider.h
@date: 24/07/2015
@author: Emmanuel Vaccaro
@brief: A capsule-shaped collider.
===============================================*/

#ifndef _CAPSULE_COLLIDER_H_
#define _CAPSULE_COLLIDER_H_

#include "Collider.h"

class CapsuleCollider : public Collider {
public:
	CapsuleCollider();
	~CapsuleCollider();
	virtual bool Startup();
	virtual bool Update();
	virtual bool Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance);
	void Inspector();

	float height;
	float radius;
};

#endif //_CAPSULE_COLLIDER_H_