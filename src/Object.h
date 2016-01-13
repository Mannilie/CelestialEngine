/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Object.h
@date: 14/06/2015
@author: Emmanuel Vaccaro
@brief: Base class for all objects
===============================================*/

#ifndef _OBJECT_H_
#define _OBJECT_H_

// Other
#include <string>
using std::string;

// Forward declaration
class RenderingEngine;
class Shader;
class Camera;

class Object {
public:
	Object();
	Object(string _name);
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;
	virtual bool Update() = 0;
	virtual void Draw(RenderingEngine& _renderer) = 0;

	static int instanceCount;
	string name;
	int	instanceID;
};

#endif //_OBJECT_H_