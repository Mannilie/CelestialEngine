/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Mesh.h
@date: 14/07/2015
@author: Emmanuel Vaccaro
@brief: A ray is an infinite line starting at 
origin and going in some direction.
===============================================*/

#ifndef _RAY_H_
#define _RAY_H_

// Utilities
#include "GLM_Header.h"

class Collider;
class Transform;

class RaycastHit {
public:
	RaycastHit() : collider(nullptr), transform(nullptr) {}
	~RaycastHit() {}

	Collider* collider; //The Collider that was hit.
	Transform* transform;
	float distance; //The distance from the ray's origin to the impact point.
	vec3 point; //The impact point in world space where the ray hit the collider.
};

class Ray {
public:
	Ray(vec3 _origin = vec3(0), vec3 _direction = vec3(0, 0, 1)) :
		origin(_origin),
		direction(_direction) {
		invDirection = vec3(1 / direction.x,
			1 / direction.y,
			1 / direction.z);
		sign[0] = (invDirection.x < 0);
		sign[1] = (invDirection.y < 0);
		sign[2] = (invDirection.z < 0);
	}
	~Ray(){}
	
	vec3 GetPoint(float _distance) { return origin + direction * _distance; } //Returns a point at distance units along the ray.
	vec3 direction; //The direction of the ray.
	vec3 origin; //The origin point of the ray.
	vec3 invDirection;
	int sign[3];
};

#endif // _RAY_H_