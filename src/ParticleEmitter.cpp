#include "ParticleEmitter.h"

// Sub-engines
#include "PhysicsEngine.h"
#include "CoreEngine.h"

// Transform
#include "Transform.h"

// GUI
#include "imgui.h"

// Debugging
#include "Debug.h"
#include "Gizmos.h"

// Other
#include "Time.h"

ParticleEmitter::ParticleEmitter(int _maxParticles) :
	maxParticles(_maxParticles),
	m_minVelocity(vec3(1.0f)),
	m_maxVelocity(vec3(10.0f)),
	restParticleDistance(0.3f),
	dynamicFriction(0.01f),
	staticFriction(0.1f),
	damping(0.01f),
	particleMass(0.01f),
	restitution(0.0f),
	stiffness(100.0f),
	enabled(false),
	isChangedInGUI(false),
	m_releaseDelay(0.1f) {}
ParticleEmitter::~ParticleEmitter(){}
bool ParticleEmitter::Startup() {
	// Allocate an array
	m_activeParticles = new FluidParticle[maxParticles]; // Array of particle structs
	m_time = 0; // Time system has been running
	m_respawnTime = 0; // Time for next respawn
	m_particleMaxAge = 8; // Maximum time in seconds that a particle can live 

	// Initialize the buffer
	for (int index = 0; index < maxParticles; index++) {
		m_activeParticles[index].isActive = false;
	}

	CoreEngine::physics->AddActor(this);

	return true;
}
void ParticleEmitter::Shutdown() {
	CoreEngine::physics->RemoveActor(this);
	// Remove all the active particles
	delete m_activeParticles;
}
bool ParticleEmitter::Update() {
	if (this->transform->isSelected) { Inspector(); }

	if (enabled) {
		// Tick the emitter
		m_time += Time::deltaTime;
		m_respawnTime += Time::deltaTime;
		int numberSpawn = 10;
		// If respawn time is greater than our release delay then we spawn at 
		// least one particle so work out how many to spawn
		if (m_respawnTime > m_releaseDelay) {
			numberSpawn = (int)(m_respawnTime / m_releaseDelay);
			m_respawnTime -= (numberSpawn * m_releaseDelay);
		}
		// Spawn the required number of particles 
		for (int count = 0; count < numberSpawn; count++) {
			// Get the next free particle
			int particleIndex = GetNextFreeParticle();
			if (particleIndex >= 0) {
				// If we got a particle ID then spawn it
				AddPhysXParticle(particleIndex);
			}
		}
	}

	// Check to see if we need to release particles. They can either be too old or have hit the particle sink
	// Lock the particle buffer so we can work on it and get a pointer to read data
	PxParticleReadData* readData = particleFluid->lockParticleReadData();
	// Access particle data from PxParticleReadData was OK
	if (readData) {
		vector<PxU32> particlesToRemove; //we need to build a list of particles to remove so we can do it all in one go
		PxStrideIterator<const PxParticleFlags> flagsIt(readData->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(readData->positionBuffer);

		for (unsigned i = 0; i < readData->validParticleRange; ++i, ++flagsIt, ++positionIt) {
			if (*flagsIt & PxParticleFlag::eVALID) {
				//if particle is either too old or has hit the sink then mark it for removal.  We can't remove it here because we buffer is locked
				if (*flagsIt & PxParticleFlag::eCOLLISION_WITH_DRAIN) {
					//mark our local copy of the particle free
					ReleaseParticle(i);
					//add to our list of particles to remove
					particlesToRemove.push_back(i);
				}
			}
		}
		// return ownership of the buffers back to the SDK
		readData->unlock();
		//if we have particles to release then pass the particles to remove to PhysX so it can release them
		if (particlesToRemove.size() > 0) {
			//create a buffer of particle indicies which we are going to remove
			PxStrideIterator<const PxU32> indexBuffer(&particlesToRemove[0]);
			//free particles from the physics system
			particleFluid->releaseParticles(particlesToRemove.size(), indexBuffer);
		}
	}

	return true;
}
void ParticleEmitter::Draw(RenderingEngine& _renderer) {
	// Lock SDK buffers of *PxParticleSystem* ps for reading
	PxParticleFluidReadData * fd = particleFluid->lockParticleFluidReadData();
	// Access particle data from PxParticleReadData
	float minX = 1000, maxX = -1000, minZ = 1000, maxZ = -1000, minY = 1000, maxY = -1000;
	if (fd) {
		PxStrideIterator<const PxParticleFlags> flagsIt(fd->flagsBuffer);
		PxStrideIterator<const PxVec3> positionIt(fd->positionBuffer);
		PxStrideIterator<const PxF32> densityIt(fd->densityBuffer);
		for (unsigned i = 0; i < fd->validParticleRange; ++i, ++flagsIt, ++positionIt, ++densityIt) {
			if (*flagsIt & PxParticleFlag::eVALID) {
				// Density tells us how many neighbours a particle has.  
				// If it has a density of 0 it has no neighbours, 1 is maximum neighbours
				// We can use this to decide if the particle is seperate or part of a larger body of fluid
				vec3 position(positionIt->x, positionIt->y, positionIt->z);
				Gizmos::AddSphere(position, restParticleDistance * 0.5f, 4, 4, vec4(1, 0, 1, 1));
			}
		}
		// return ownership of the buffers back to the SDK
		fd->unlock();
	}
	Gizmos::AddTransform(this->transform->worldMatrix);
}
void ParticleEmitter::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("ParticleEmitter")) {
		float oldGUIRestParticleDistance = restParticleDistance;
		float oldGUIDynamicFriction = dynamicFriction;
		float oldGUIStaticFriction = staticFriction;
		float oldGUIDamping = damping;
		float oldGUIParticleMass = particleMass;
		float oldGUIRestitution = restitution;
		float oldGUIStiffness = stiffness;
		bool oldGUIEnabled = enabled;

		// Settings go here
		ImGui::DragFloat("Rest Particle Distance", &restParticleDistance, 0.01f);
		ImGui::DragFloat("Dynamic Friction", &dynamicFriction, 0.01f);
		ImGui::DragFloat("Static Friction", &staticFriction, 0.01f);
		ImGui::DragFloat("Damping", &damping, 0.01f);
		ImGui::DragFloat("Particle Mass", &particleMass, 0.01f);
		ImGui::DragFloat("Restitution", &restitution, 0.01f);
		ImGui::DragFloat("Stiffness", &stiffness, 0.01f);
		ImGui::Checkbox("Enabled", &enabled);

		ImGui::TreePop();

		if (oldGUIRestParticleDistance != restParticleDistance ||
			oldGUIDynamicFriction != dynamicFriction ||
			oldGUIStaticFriction != staticFriction ||
			oldGUIDamping != damping ||
			oldGUIParticleMass != particleMass ||
			oldGUIRestitution != restitution ||
			oldGUIStiffness != stiffness ||
			oldGUIEnabled != enabled) {
			isChangedInGUI = true;
		} else {
			isChangedInGUI = false;
		}
	}
	ImGui::EndChild();
	ImGui::End();
}
void ParticleEmitter::SetVelocityRange(float _minX, float _minY, float _minZ, 
	float _maxX, float _maxY, float _maxZ) {
	m_minVelocity.x = _minX;
	m_minVelocity.y = _minY;
	m_minVelocity.z = _minZ;
	m_maxVelocity.x = _maxX;
	m_maxVelocity.y = _maxY;
	m_maxVelocity.z = _maxZ;
}
int ParticleEmitter::GetNextFreeParticle() {
	// Loop through particles. 
	// Note(Manny): Might be a better way to do this.
	for (int particleIndex = 0; particleIndex < maxParticles; particleIndex++) {
		// Find a particle which is free
		if (!m_activeParticles[particleIndex].isActive) {
			// Mark it as not free
			m_activeParticles[particleIndex].isActive = true; 
	
			// Record when the particle was created so we know when to remove it
			m_activeParticles[particleIndex].maxTime = m_time + m_particleMaxAge;  
			return particleIndex;
		}
	}
	// Returns -1 if a particle was not allocated
	return -1;
}
void ParticleEmitter::ReleaseParticle(int _particleIndex) {
	// Release a particle from the system using it's index to ID it
	if (_particleIndex >= 0 && _particleIndex < maxParticles) {
		m_activeParticles[_particleIndex].isActive = false;
	}
}
// Returns true if a particle age is greater than it's maximum allowed age
bool ParticleEmitter::isTooOld(int _particleIndex) {
	if (_particleIndex >= 0 && _particleIndex < maxParticles &&
		m_time > m_activeParticles[_particleIndex].maxTime) {
		return true;
	}
	return false;
}
// Add particle to PhysX System
bool ParticleEmitter::AddPhysXParticle(int _particleIndex) {
	PxParticleCreationData particleCreationData;
	
	// Spawn one particle at a time.
	// Note(Manny): This is inefficient and we could improve this by passing in the list of particles.
	particleCreationData.numParticles = 1;
	// Set up the buffers
	PxU32 myIndexBuffer[] = { _particleIndex };
	PxVec3 startPos = PxVec3(this->transform->position.x, 
							 this->transform->position.y, 
							 this->transform->position.z);
	
	PxVec3 startVel((float)(rand() % (int)m_maxVelocity.x + (int)m_minVelocity.x), 
					(float)(rand() % (int)m_maxVelocity.y + (int)m_minVelocity.y), 
					(float)(rand() % (int)m_maxVelocity.z + (int)m_minVelocity.z));

	// Randomize starting velocity.
	float fT = (rand() % (RAND_MAX + 1)) / (float)RAND_MAX;
	startVel.x += m_minVelocity.x + (fT * (m_maxVelocity.x - m_minVelocity.x));
	fT = (rand() % (RAND_MAX + 1)) / (float)RAND_MAX;
	startVel.y += m_minVelocity.y + (fT * (m_maxVelocity.y - m_minVelocity.y));
	fT = (rand() % (RAND_MAX + 1)) / (float)RAND_MAX;
	startVel.z += m_minVelocity.z + (fT * (m_maxVelocity.z - m_minVelocity.z));

	// We can change starting position tos get different emitter shapes
	PxVec3 myPositionBuffer[] = { startPos };
	PxVec3 myVelocityBuffer[] = { startVel };

	particleCreationData.indexBuffer = PxStrideIterator<const PxU32>(myIndexBuffer);
	particleCreationData.positionBuffer = PxStrideIterator<const PxVec3>(myPositionBuffer);
	particleCreationData.velocityBuffer = PxStrideIterator<const PxVec3>(myVelocityBuffer);
	// Create particles in *PxParticleSystem* ps
	return particleFluid->createParticles(particleCreationData);
}