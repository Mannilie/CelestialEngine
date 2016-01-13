/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: LineSegment.h
@date: 05/07/2015
@author: Emmanuel Vaccaro
@brief: Represents a line segment in 3D space
===============================================*/

#ifndef _LINE_SEGMENT_H_
#define _LINE_SEGMENT_H_

// Utilities
#include "GLM_Header.h"

class LineSegment {
public:
	LineSegment();
	LineSegment(const vec3& _start, const vec3& _end);
	~LineSegment();
	void Draw(vec4 _color);
	
	vec3 start;
	vec3 end;
};

#endif // _LINE_SEGMENT_H_