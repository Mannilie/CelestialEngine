/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Bounds.h
@date: 03/07/2015
@author: Emmanuel Vaccaro
@brief: Represents an axis aligned bounding box.
===============================================*/

#ifndef _BOUNDS_H_
#define _BOUNDS_H_

// Utilities
#include "GLM_Header.h"

// Forward declaration
class Ray;
class RaycastHit;

class Bounds {
public:
	Bounds();
	Bounds(vec3 _center, vec3 _size);
	~Bounds();
	bool Startup();
	bool Update();
	void Draw();
	bool Contains(vec3 _point); // Is point contained in the bounding box?
	bool IntersectRay(Ray& _ray); // Does ray intersect this bounding box?
	bool RayCast(Ray& _ray, RaycastHit* _hitInfo, float _maxDistance);
	bool Intersects(Bounds& _bounds); // Does another bounding box intersect with this bounding box?
	void SetMinMax(vec3 _min, vec3 _max); // Sets the bounds to the min and max value of the box.
	vec3 operator[](int _index);
	void Inspector();
	bool HasChanged();

	vec3 center;
	vec3 extents;
	vec3 max; //center + extents
	vec3 min; //center - extents
	vec3 size; //extents * 2 (size.x = width, size.y = height, size.z = depth)	
	mutable vec3 oldMin;
	mutable vec3 oldMax;
	mutable vec3 oldExtents;
	mutable vec3 oldSize;
	mutable bool initializedOldStuff;
};


#endif // _BOUNDS_H_