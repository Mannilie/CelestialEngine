/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Time.h
@date: 16/06/2015
@author: Emmanuel Vaccaro
@brief: Container that stores and calculates
time.
===============================================*/

#ifndef _TIME_H_
#define _TIME_H_

// Utilities
#include "GLFW_Header.h"

class Time
{
public:
	static Time* instance;
	static float deltaTime;
	static float elapsedTime;
	static float fps;
	
	Time();
	static void Create();
	static bool Update();
};

#endif // _TIME_H_