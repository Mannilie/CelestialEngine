/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: CoreEngine.h
@date: 13/06/2015
@author: Emmanuel Vaccaro
@brief: The core engine handles all of the 
sub-engines within the tool
===============================================*/

#ifndef _CORE_ENGINE_H_
#define _CORE_ENGINE_H_

// Utilities
#include "GLFW_Header.h"

class Game;
class RenderingEngine;
class PhysicsEngine;

class CoreEngine {
public:
	CoreEngine(RenderingEngine* _renderer, PhysicsEngine* _physics, Game* _game);
	bool Startup();
	void Shutdown();
	bool Update();
	void Draw();
	
	bool physicsEnabled;
	RenderingEngine* renderer;
	static PhysicsEngine* physics;
	Game* game;
};

#endif // _CORE_ENGINE_H_