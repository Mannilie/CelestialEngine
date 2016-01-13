/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Fluid.h
@date: 26/07/2015
@author: Emmanuel Vaccaro
@brief: Creates fluid simulation
===============================================*/

#ifndef _FLUID_H_
#define _FLUID_H_

// Components
#include "Component.h"
#include "Transform.h"

// Structs
#include "Mesh.h"

// Utilities
#include "GLM_Header.h"

struct FluidCell {
	float* pressure;
	vec2* velocity;
	vec3* dyeColor;
};

class RenderingEngine;
class Fluid : public Component {
public:
	Fluid(int _width = 64, int _height = 64,
		float _viscocity = 0.1f, float _cellDist = 0.1f);
	~Fluid();
	bool Startup();
	void Shutdown();
	bool Update();
	void Draw(RenderingEngine& _renderer);
	void Advect(float _deltaTime);
	void Diffuse(float _deltaTime);
	void Divergence(float _deltaTime);
	void UpdatePressure(float _deltaTime);
	void ApplyPressure(float _deltaTime);
	void UpdateBoundary();
	void SwapVelocities();
	void SwapColors();
	void SwapPressures();

	FluidCell frontCells;
	FluidCell backCells;
	float viscocity;
	float cellDist;
	int width, height;
	int cellCount;

	float* divergence;
};

#endif // _FLUID_H_