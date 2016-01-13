#ifndef _FIRST_PERSON_CAMERA_SCRIPT_H_
#define _FIRST_PERSON_CAMERA_SCRIPT_H_

#include "Component.h"
#include "RenderingEngine.h"

class FirstPersonCameraScript : public Component
{
public:
	FirstPersonCameraScript();

	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw(RenderingEngine& _renderer);

	void Inspector();

	float maxY;
	float minY;
	float mouseSensitivity;
	float jumpHeight;

	mutable double oldMouseX;
	mutable double oldMouseY;
};

#endif //_MESH_RENDERER_H_