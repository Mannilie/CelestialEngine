/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Main.cpp
@date: 13/06/2015
@author: Emmanuel Vaccaro
@brief: Entry-point into the Game Engine
===============================================*/

// Sub-engines
#include "GLFW_Header.h"
#include "RenderingEngine.h"
#include "CustomPhysicsEngine.h"
#include "PhysXEngine.h"
#include "CoreEngine.h"

// Debugging
#include "Debug.h"

// Other
#include "Scene.h"

CoreEngine* g_gameEngine;

bool RunCoreEngine();
void OnWindowResize(GLFWwindow* _window, int _width, int _height);
void OnFrameBufferResize(GLFWwindow* _window, int _width, int _height);

int main() {
	// Seed the randomizer for the engine
	srand((unsigned int)time(NULL));

	// Create a window
	Window::Create(1560, 1024, "GameEngine_EV"); 
	
	// Initialize debugger
	Debug::Create();

	// Create sub-engines
	RenderingEngine renderer;
	PhysXEngine physicsEngine;
	
	// Create the scene
	Scene scene;

	// Create the core engine
	CoreEngine gameEngine(&renderer, &physicsEngine, &scene);

	// Start up the engine
	g_gameEngine = &gameEngine;
	if (gameEngine.Startup() == false) {
		return -1; // Exited with error -1
	}
	
	// Setup resizing callbacks and framebuffer handles
	glfwSetWindowSizeCallback(Window::window, OnWindowResize);
	glfwSetFramebufferSizeCallback(Window::window, OnFrameBufferResize);

	// Game loop
	while (gameEngine.Update()) {
		gameEngine.Draw(); 
	}

	// Shutdown the engine
	gameEngine.Shutdown();

	// Exited successfully
	return 0;
}

// Refreshes game engine whilst resizing
bool RunCoreEngine() {
	if (g_gameEngine->Update() == false) {
		return false;
	}
	g_gameEngine->Draw();
	return true;
}
void OnWindowResize(GLFWwindow* _window, int _width, int _height) {
	RunCoreEngine();
}
void OnFrameBufferResize(GLFWwindow* _window, int _width, int _height) {
	RunCoreEngine();
}