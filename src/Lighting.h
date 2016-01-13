/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Lighting.h
@date: 04/07/2015
@author: Emmanuel Vaccaro
@brief: Lights determine the shading of an 
object and use the rendering engine to 
achieve their final result.
===============================================*/

#ifndef _LIGHTING_H_
#define _LIGHTING_H_

// Maths
#include "GLM_Header.h"

// Components
#include "Component.h"

struct Attenuation {
	float constant;
	float linear;
	float quadratic;
	Attenuation(float _constant = 0, float _linear = 0, float _quadratic = 1) :
		constant(_constant), 
		linear(_linear), 
		quadratic(_quadratic) {}
};

class DirectionalLight : public Component {
public:
	DirectionalLight(vec3 _direction = vec3(0, -1, 0), 
		const vec3& _ambient = vec3(0.5f), 
		const vec3& _diffuse = vec3(0.8f), 
		const vec3& _specular = vec3(1.0f));
	virtual bool Startup();
	virtual void Shutdown(){}
	virtual bool Update();
	virtual void Draw(Shader& _shader, RenderingEngine& _renderer, const Camera& _camera){}
	void Inspector();

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

class PointLight : public Component {
public:
	PointLight(const Attenuation& _attenuation = Attenuation(),
		const vec3& _ambient = vec3(0.05f), 
		const vec3& _diffuse = vec3(0.8f), 
		const vec3& _specular = vec3(1.0f));
	virtual bool Startup();
	virtual void Shutdown(){}
	virtual bool Update();
	virtual void Draw(Shader& _shader, RenderingEngine& _renderer, const Camera& _camera){}
	void Inspector();

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	Attenuation attenuation;
};

// Note(Manny): Add Spot light

#endif //_LIGHTING_H_