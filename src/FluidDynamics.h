#ifndef _FLUID_DYNAMICS_H_
#define _FLUID_DYNAMICS_H_

#include "Component.h"

class FluidDynamics : public Component
{
public:
	FluidDynamics();
	virtual ~FluidDynamics();

	bool Startup();
	void Shutdown();
	bool Update();
	void Draw(RenderingEngine& _renderer);

};

#endif //_FLUID_DYNAMICS_H_