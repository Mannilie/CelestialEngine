/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: MeshCollider.h
@date: 25/07/2015
@author: Emmanuel Vaccaro
@brief: A collider that takes the shape of a
mesh.
===============================================*/

#ifndef _MESH_COLLIDER_H_
#define _MESH_COLLIDER_H_

#include "Collider.h"

// Structs
#include "Mesh.h"

class MeshCollider : public Collider {
public:
	MeshCollider();
	~MeshCollider();
	virtual bool Startup();
	virtual bool Update();
	virtual bool Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance);
	void Inspector();

	Mesh* meshObject;
};

#endif // _MESH_COLLIDER_H_