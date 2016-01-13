/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Vertex.h
@date: 16/06/2015
@author: Emmanuel Vaccaro
@brief: Definition for vertices. 
===============================================*/

#ifndef _VERTEX_H_
#define _VERTEX_H_

// Utilities
#include "GLM_Header.h"

struct Vertex {
	vec4 position;
	vec4 color;
};

struct VertexTexCoord {
	vec4 position;
	vec2 texCoord;
};

struct VertexNormal {
	vec4 position;
	vec4 normal;
	vec4 tangent;
	vec2 texCoord;
};

struct OpenGLData {
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;
	unsigned int indexCount;
};

#endif // _VERTEX_H_