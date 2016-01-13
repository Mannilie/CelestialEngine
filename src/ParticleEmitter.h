/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: ParticleEmitter.h
@date: 14/07/2015
@author: Emmanuel Vaccaro
@brief: Emitts particles using both the 
RenderingEngine and the PhysXEngine
===============================================*/

#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

// Sub-engines
#include "PhysXEngine.h"

// Physics
#include "PhysicsObject.h"

struct FluidParticle {
	bool isActive;
	float maxTime;
};

class ParticleEmitter : public PhysicsObject {
public:
	ParticleEmitter(int _maxParticles = 4000);
	~ParticleEmitter();
	bool Startup();
	void Shutdown();
	bool Update();
	void Draw(RenderingEngine& _renderer);
	void ReleaseParticle(int _particleIndex);
	bool isTooOld(int _particleIndex);
	void SetVelocityRange(float _minX, float _minY, float _minZ,
		float _maxX, float _maxY, float _maxZ);
	void Inspector();

	int	maxParticles;
	float restParticleDistance;
	float dynamicFriction;
	float staticFriction;
	float damping;
	float particleMass;
	float restitution;
	float stiffness;
	bool enabled;
	bool isChangedInGUI;
	PxParticleFluid* particleFluid;

private:
	int GetNextFreeParticle();
	bool AddPhysXParticle(int _particleIndex);

	vec3 m_minVelocity;
	vec3 m_maxVelocity;
	int m_rows;
	int m_cols;
	int m_depth;
	int m_numberActiveParticles;
	int m_boxWidth;
	int m_boxHeight;
	float m_releaseDelay;
	float m_time;
	float m_respawnTime;
	float m_particleMaxAge;
	FluidParticle* m_activeParticles;
};

#endif // _PARTICLE_EMITTER_H_