#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//Note(Manny): NOT IMPLEMENTED!

#include "Component.h"
#include "RenderingEngine.h"

class Animation : public Component
{
public:
	Animation();
	~Animation();

	virtual bool Startup();
	virtual void Shutdown(){}
	virtual bool Update();
	virtual void Draw(RenderingEngine& _renderer);

	void Inspector();
};

#endif //_MESH_RENDERER_H_