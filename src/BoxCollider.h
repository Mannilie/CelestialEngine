/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: BoxCollider.h
@date: 24/07/2015
@author: Emmanuel Vaccaro
@brief: A box-shaped primitive collider.
===============================================*/

#ifndef _BOX_COLLIDER_H_
#define _BOX_COLLIDER_H_

#include "Collider.h"

// Structs
#include "Bounds.h"
#include "OBB.h"

class BoxCollider : public Collider {
public:
	BoxCollider();
	~BoxCollider();
	virtual bool Startup();
	virtual bool Update();	
	virtual bool Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance);
	void Inspector();

	OBB obb;
};

#endif //_BOX_COLLIDER_H_