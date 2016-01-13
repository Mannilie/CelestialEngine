/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: FlyCameraScript.h
@date: 16/07/2015
@author: Emmanuel Vaccaro
@brief: A script that lets the user control
a scene camera.
===============================================*/

#ifndef _FLY_CAMERA_SCRIPT_H_
#define _FLY_CAMERA_SCRIPT_H_

// Sub-engines
#include "RenderingEngine.h"

// Components
#include "Component.h"

class FlyCameraScript : public Component {
public:
	FlyCameraScript();
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	void Inspector();

	float mouseSensitivity;
	float movementSpeed;
	mutable double oldMouseX;
	mutable double oldMouseY;
};

#endif // _FLY_CAMERA_SCRIPT_H_