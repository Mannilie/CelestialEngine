/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Window.h
@date: 13/06/2015
@author: Emmanuel Vaccaro
@brief: A manager for creating windows.
===============================================*/

#ifndef _WINDOW_H_
#define _WINDOW_H_

// Other
#include <string>
using std::string;

struct GLFWwindow;

class Window {
public:
	static Window* instance;

	static GLFWwindow* window;
	static int width;
	static int height;
	static string title;
	static float xScroll;
	static float yScroll;

	Window() {}
	static bool Create(int _width = width, int _height = height, string _title = title);
	static bool IsCloseRequested();
	static void Shutdown();
	static bool Update();
	static void Draw();
	static void BindAsRenderTarget();
};

#endif // _WINDOW_H_