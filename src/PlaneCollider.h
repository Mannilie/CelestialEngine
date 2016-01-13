/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: PlaneCollider.h
@date: 25/07/2015
@author: Emmanuel Vaccaro
@brief: A plane-shaped primitive collider.
===============================================*/

#ifndef _PLANE_COLLIDER_H_
#define _PLANE_COLLIDER_H_

// Components
#include "Collider.h"

class PlaneCollider : public Collider {
public:
	PlaneCollider();
	~PlaneCollider();
	virtual bool Startup();
	virtual bool Update();
	void Inspector();

	vec3 normal;
	float distance;
};

#endif //_PLANE_COLLIDER_H_