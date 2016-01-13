/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Component.h
@date: 17/06/2015
@author: Emmanuel Vaccaro
@brief: A base class for everything that 
attaches to GameObjects
===============================================*/

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class GameObject;
class Transform;

class RenderingEngine;
class Shader;
class Camera;

class Component {
public:
	Component() : gameObject(nullptr), transform(nullptr) {}
	virtual bool Startup(){ return true; }
	virtual void Shutdown(){}
	virtual bool Update() = 0;
	virtual void Draw(RenderingEngine& _renderer){}

	GameObject* gameObject;
	Transform*  transform;
};

#endif //_COMPONENT_H_