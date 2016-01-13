/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: GUI.h
@date: 23/06/2015
@author: Emmanuel Vaccaro
@brief: Handles Graphical User Interface within
the engine
===============================================*/

#ifndef _GUI_H_
#define _GUI_H_

// Components
#include "Transform.h"

// GUI
#include "imgui.h"

// Utilities
#include "GLFW_Header.h"

// Other
#include <iostream>

class GUI {
public:
	static void mouseButtonCallback(GLFWwindow*, int _button, int _action, int /*mods*/);
	static void keyCallback(GLFWwindow*, int _key, int, int _action, int _mods);
	static void charCallback(GLFWwindow*, unsigned int _c);

	GUI();
	~GUI();
	static void Create();
	static void Shutdown();
	static void Update(); //This MUST happen before anything else updates!
	static void Draw(); //This MUST be called last!
	static void CreateFontsTexture();
	static bool CreateDeviceObjects();

	static bool	mousePressed[3];
	static GLuint fontTexture;
	static int shaderHandle;
	static int vertHandle;
	static int fragHandle;
	static int attribLocationTex;
	static int attribLocationProjMtx;
	static int attribLocationPosition;
	static int attribLocationUV;
	static int attribLocationColor;
	static size_t vboSize;
	static unsigned int vboHandle;
	static unsigned int vaoHandle;
};

#endif // _GUI_H_