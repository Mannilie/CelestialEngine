#include "PhysXEngine.h"

#include "GLM_Header.h"

// Colliders
#include "Collider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "PlaneCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"

// Components
#include "CharacterController.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Ragdoll.h"
#include "ParticleEmitter.h"

// PhysX
#include <physx\Include\characterkinematic\PxCapsuleController.h>
#include <physx\Include\foundation\PxErrors.h>
#include <physx\Include\cooking\PxCooking.h>

// Debugging
#include "Gizmos.h"
#include "Debug.h"

// Other
#include "Time.h"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

class PhysXAllocatorCallback : public PxAllocatorCallback {
public:
	virtual ~PhysXAllocatorCallback(){}
	virtual void* allocate(size_t _bytes, const char* _typeName, const char* _fileName, int _line) {
		void* ptr = _aligned_malloc(_bytes, 16);
		return ptr;
	}
	virtual void deallocate(void* _ptr) {
		_aligned_free(_ptr);
	}
};

class PhysXErrorCallback : public PxErrorCallback {
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) {
		switch (code) {
		case physx::PxErrorCode::eNO_ERROR: break;
		case physx::PxErrorCode::eDEBUG_INFO:
			Debug::Log("PhysX. Info: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eDEBUG_WARNING:
			Debug::LogWarning("PhysX warning. Warning: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eINVALID_PARAMETER:
			Debug::LogError("PhysX invalid parameter error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eINVALID_OPERATION:
			Debug::LogError("PhysX invalid operation error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eOUT_OF_MEMORY:
			Debug::LogError("PhysX memory error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eINTERNAL_ERROR:
			Debug::LogError("PhysX internal error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eABORT:
			Debug::LogError("PhysX abort error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::ePERF_WARNING:
			Debug::LogWarning("PhysX performance warning. Warning: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		case physx::PxErrorCode::eMASK_ALL:
			Debug::LogError("PhysX mask error. Error: '" + string(message) + "' File: '" + string(file) + "' Line: " + to_string(line));
			break;
		default:
			break;
		}
	}
};

class PhysXControllerHitReportCallback : public PxUserControllerHitReport {
public:
	// Overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit) {
		// Get the normal of the thing we hit and store it so the player controller can respond correctly
		_playerContactNormal = hit.worldNormal;
		// Gets a reference to a structure which tells us what has been hit and where

		// Get the acter from the shape we hit
		PxRigidActor* actor = hit.shape->getActor();
		if (actor) {
			// Try to cast to a dynamic actor
			PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
			if (myActor) {
				// This is where we can apply forces to things we hit
			}
		}
	}
	// Other collision functions which we must overload //these handle collision with other controllers and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit){};
	// Called when current controller hits another controller. More...
	virtual void onObstacleHit(const PxControllerObstacleHit &hit){};
	// Called when current controller hits a user-defined obstacl
	PhysXControllerHitReportCallback() : PxUserControllerHitReport(){};
	PxVec3 getPlayerContactNormal(){ return _playerContactNormal; };
	void clearPlayerContactNormal(){ _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;
};

bool PhysXEngine::Startup() {
	PxErrorCallback* errorCallback = new PhysXErrorCallback();
	PxAllocatorCallback* allocatorCallback = new PhysXAllocatorCallback();
	defaultFilterShader = PxDefaultSimulationFilterShader;

	// PhysicsFoundation
	physicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *allocatorCallback, *errorCallback);
	if (!physicsFoundation) {
		Debug::LogError("Function failure 'PxCreateFoundation' in 'PhysXEngine::Startup'");
	}

	// ProfileZoneManager
	profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(physicsFoundation);
	if (!profileZoneManager) {
		Debug::LogError("Function failure 'PxProfileZoneManager::createProfileZoneManager' in 'PhysXEngine::Startup'");
	}

	// Physics
	bool RecordMemoryAllocations = true;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *physicsFoundation, PxTolerancesScale(), RecordMemoryAllocations, profileZoneManager);
	if (!physics) {
		Debug::LogError("Function failure 'PxCreatePhysics' in 'PhysXEngine::Startup'");
	}

	// PhysicsCooker
	physicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *physicsFoundation, PxCookingParams(PxTolerancesScale()));
	if (!physicsCooker) {
		Debug::LogError("Function failure 'PxCreateCooking' in 'PhysXEngine::Startup'");
	}

	if (!PxInitExtensions(*physics)) {
		Debug::LogError("Function failure 'PxInitExtensions' in 'PhysXEngine::Startup'");
	}

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(gravity.x, gravity.y, gravity.z);
	sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	
	// Create CPU dispatcher which mNbThreads worker threads
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);

#ifdef PX_WINDOWS
	PxCudaContextManagerDesc cudaContextManagerDesc;
	cudaContextManager = PxCreateCudaContextManager(*physicsFoundation, cudaContextManagerDesc, profileZoneManager);
	if (!cudaContextManager) {
		Debug::LogError("Function failure 'PxCreateCudaContextManager' in 'PhysXEngine::Startup'");
	}
	sceneDesc.gpuDispatcher = cudaContextManager->getGpuDispatcher();
#endif

	physicsScene = physics->createScene(sceneDesc);
	
	physicsMaterial = physics->createMaterial(1.0f, 0.5f, 0.5f);

	ragdollMaterial = physics->createMaterial(1.0f, 0.5f, 0.5f);

	// Create character manager
	characterManager = PxCreateControllerManager(*physicsScene);

	SetupVisualDebugger();

	characterYVelocity = 0;

	return true;
}

void PhysXEngine::Shutdown() {
	actors.clear();
	characterManager->release();
	PxCloseExtensions();
	physicsCooker->release();
	physics->release();
	cudaContextManager->release();
	physicsFoundation->release();
}

bool PhysXEngine::Update() {
	if (Time::deltaTime <= 0) return true;

	physicsScene->simulate(Time::deltaTime > 0.033f ? 0.033f : Time::deltaTime);
	while (physicsScene->fetchResults() == false);

	for (unsigned int i = 0; i < actors.size(); ++i) {
		PhysXActor* actor = &actors[i];
		PhysicsObject* physicsObject = actors[i].physicsObject;
		PxActor* pxActor = actors[i].pxActor;
		
		GameObject* gameObject = physicsObject->gameObject;

		if (pxActor->isRigidStatic()) {
			PxRigidStatic* rigidStatic = pxActor->is<PxRigidStatic>();
			UpdateActorTransform(actor);
		} else if (pxActor->isRigidDynamic()) {
			PxRigidDynamic* rigidDynamic = pxActor->is<PxRigidDynamic>();
			
			PxTransform transform = rigidDynamic->getGlobalPose();
			vec3 position = vec3(transform.p.x, transform.p.y, transform.p.z);
			quat rotation = quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

			// Transform
			UpdateActorTransform(actor);
			
			// Rigidbody
			Rigidbody* rigidbody = gameObject->GetComponent<Rigidbody>();
			if (rigidbody) { UpdateActorRigidbody(actor); }

			// Character Controller
			CharacterController* characterController = gameObject->GetComponent<CharacterController>();
			if (characterController) { UpdateActorCharacterController(actor); }
			
			// Particle Emitter
			ParticleEmitter* particleEmitter = gameObject->GetComponent<ParticleEmitter>();
			if (particleEmitter) { UpdateActorParticleEmitter(actor); }
		}
	}

	for (unsigned int i = 0; i < articulations.size(); ++i) {
		PhysXArticulation* articulation = &articulations[i];
		PhysicsObject* physicsObject = articulations[i].physicsObject;
		PxArticulation* pxArticulation = articulations[i].pxArticulation;
		GameObject* gameObject = physicsObject->gameObject;

		// Ragdoll
		Ragdoll* ragdoll = gameObject->GetComponent<Ragdoll>();
		if (ragdoll) {
			UpdateArticulationRagdoll(articulation);
		}
	}

	return true;
}

// Actor
bool PhysXEngine::UpdateActorTransform(PhysXActor* _actor) {
	GameObject* gameObject = _actor->physicsObject->gameObject;
	PxActor* pxActor = _actor->pxActor;
	ShapeID& shapeId = _actor->shapeId;

	if (!pxActor->isRigidActor()) {
		Debug::LogError("PxActor must be of type 'PxRigidDynamic' to call function 'PhysXEngine::UpdateActorTransform'");
		return false;
	}

	PxRigidActor* rigidActor = pxActor->is<PxRigidActor>();

	PxTransform transform = rigidActor->getGlobalPose();
	vec3 position = vec3(transform.p.x, transform.p.y, transform.p.z);
	quat rotation = quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

	if (gameObject->transform.HasChangedInGUI()) {
		transform.p.x = gameObject->transform.position.x;
		transform.p.y = gameObject->transform.position.y;
		transform.p.z = gameObject->transform.position.z;

		transform.q.w = gameObject->transform.rotation.w;
		transform.q.x = gameObject->transform.rotation.x;
		transform.q.y = gameObject->transform.rotation.y;
		transform.q.z = gameObject->transform.rotation.z;

		rigidActor->setGlobalPose(transform);

		if (shapeId == SHAPE_MESH) {
			PxMeshScale scale(PxVec3(glm::max(0.001f, gameObject->transform.scale.x),
				glm::max(0.001f, gameObject->transform.scale.y),
				glm::max(0.001f, gameObject->transform.scale.z)), PxQuat(1.0f));

			PxShape* shapes[1];
			rigidActor->getShapes(shapes, 1, 0);
			PxShape* oldShape = shapes[0];

			PxTriangleMeshGeometry geom;
			oldShape->getTriangleMeshGeometry(geom);
			geom.scale = scale;

			PxShape* newShape = physics->createShape(PxTriangleMeshGeometry(geom.triangleMesh, scale), *playerPhysicsMaterial);

			rigidActor->detachShape(*oldShape);
			rigidActor->attachShape(*newShape);
		}
	} else {
		gameObject->transform.position = position;
		gameObject->transform.rotation = rotation;
		gameObject->transform.eulerAngles = glm::eulerAngles(rotation);
	}

	return true;
}

bool PhysXEngine::UpdateActorParticleEmitter(PhysXActor* _actor) {
	GameObject* gameObject = _actor->physicsObject->gameObject;
	ParticleEmitter* particleEmitter = gameObject->GetComponent<ParticleEmitter>();
	PxActor* pxActor = _actor->pxActor;

	if (!particleEmitter) {
		Debug::LogError("Cannot call 'PhysXEngine::UpdateActorParticleEmitter' on an actor without a 'ParticleEmitter' component");
		return false;
	}

	if (!pxActor->isRigidDynamic()) {
		Debug::LogError("PxActor must be of type 'PxRigidDynamic' to call function 'PhysXEngine::UpdateActorParticleEmitter'");
		return false;
	}

	PxRigidDynamic* rigidDynamic = pxActor->is<PxRigidDynamic>();

	PxTransform transform = rigidDynamic->getGlobalPose();
	vec3 position = vec3(transform.p.x, transform.p.y, transform.p.z);
	quat rotation = quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

	ParticleEmitter& pE = *particleEmitter;
	PxParticleFluid* particleFluid = pE.particleFluid;

	if (particleEmitter->isChangedInGUI) {
		// Particle emitter settings
		particleFluid->setRestParticleDistance(pE.restParticleDistance);
		particleFluid->setDynamicFriction(pE.dynamicFriction);
		particleFluid->setStaticFriction(pE.staticFriction);
		particleFluid->setDamping(pE.damping);
		particleFluid->setParticleMass(pE.particleMass);
		particleFluid->setRestitution(pE.restitution);
		particleFluid->setStiffness(pE.stiffness);
	}
	
	return true;
}

bool PhysXEngine::UpdateActorRigidbody(PhysXActor* _actor) {
	Rigidbody* rigidbody = _actor->physicsObject->gameObject->GetComponent<Rigidbody>();
	PxActor* pxActor = _actor->pxActor;

	if (!rigidbody) {
		Debug::LogError("Cannot call 'PhysXEngine::UpdateActorRigidbody' on an actor without a 'Rigidbody' component");
		return false;
	}

	if (!pxActor->isRigidDynamic()) {
		Debug::LogError("PxActor must be of type 'PxRigidDynamic' to call function 'PhysXEngine::UpdateActorRigidbody'");
		return false;
	}

	PxRigidDynamic* rigidDynamic = pxActor->is<PxRigidDynamic>();

	if (rigidbody->HasChangedInGUI()) {
		rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !rigidbody->useGravity);
		rigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidbody->isKinematic);
		rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !rigidbody->enabled);
	} else {
		// Velocity
		PxVec3 linearVelocity = rigidDynamic->getLinearVelocity();
		rigidbody->velocity.x = linearVelocity.x;
		rigidbody->velocity.y = linearVelocity.y;
		rigidbody->velocity.z = linearVelocity.z;
	}	

	return true;
}

bool PhysXEngine::UpdateActorCharacterController(PhysXActor* _actor) { 
	GameObject* gameObject = _actor->physicsObject->gameObject;
	CharacterController* controller = gameObject->GetComponent<CharacterController>();
	PxActor* pxActor = _actor->pxActor;
	
	if (!controller) {
		Debug::LogError("Cannot call 'PhysXEngine::UpdateActorCharacterController' on an actor without a 'CharacterController' component");
		return false;
	}

	if (!pxActor->isRigidDynamic()) {
		Debug::LogError("PxActor must be of type 'PxRigidDynamic' to call function 'PhysXEngine::UpdateActorCharacterController'");
		return false;
	}

	PxRigidDynamic* rigidDynamic = pxActor->is<PxRigidDynamic>();

	PxTransform transform = rigidDynamic->getGlobalPose();
	vec3 position = vec3(transform.p.x, transform.p.y, transform.p.z);
	quat rotation = quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

	if (controller->enabled) {
		if (controller->isChangedInGUI) {
			playerController->resize(glm::max(0.1f, controller->height));
			playerController->setSlopeLimit(controller->slopeLimit);
			playerController->setStepOffset(controller->stepOffset);

			rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !controller->useGravity);
			rigidDynamic->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, !controller->enabled);
		}

		if (gameObject->transform.HasChangedInGUI()) {
			playerController->setPosition(PxExtendedVec3(gameObject->transform.position.x,
														 gameObject->transform.position.y,
														 gameObject->transform.position.z));
		} else {
			float minDistance = 0.001f;
			PxControllerFilters filter;

			vec3& velocity = controller->velocity;
			float& gravity = controller->gravity;
			bool& isGrounded = controller->isGrounded;

			PxQuat pxRotation(controller->yRotation, PxVec3(0, 1, 0));
			PxVec3 pxVelocity(velocity.x, velocity.y, velocity.z);

			playerController->move(pxVelocity, minDistance, Time::deltaTime, filter);

			rotation = quat(pxRotation.w, pxRotation.x, pxRotation.y, pxRotation.z);

			gameObject->transform.position = position;
			gameObject->transform.rotation = rotation;
			gameObject->transform.eulerAngles = glm::eulerAngles(rotation);

			if (characterHitReport->getPlayerContactNormal().y > 0.3f) {
				velocity.y = -0.1f;
				isGrounded = true;
			} else {
				velocity.y += gravity * Time::deltaTime;
				isGrounded = false;
			}
			characterHitReport->clearPlayerContactNormal();

		}
	}
	return true;
}

// Articulation
bool PhysXEngine::UpdateArticulationRagdoll(PhysXArticulation* _articulation) {
	GameObject* gameObject = _articulation->physicsObject->gameObject;
	Ragdoll* ragdoll = gameObject->GetComponent<Ragdoll>();
	PxArticulation* pxArticulation = _articulation->pxArticulation;

	if (!ragdoll) {
		Debug::LogError("Cannot call 'PhysXEngine::UpdateArticulationRagdoll' on an actor without a 'Ragdoll' component");
		return false;
	}

	PxU32 linkAmount = pxArticulation->getNbLinks();
	PxArticulationLink** links = new PxArticulationLink*[linkAmount];
	pxArticulation->getLinks(links, linkAmount);
	while (linkAmount--) {
		PxArticulationLink* link = links[linkAmount];
		PxU32 shapeAmount = link->getNbShapes();
		PxShape** shapes = new PxShape*[shapeAmount];
		link->getShapes(shapes, shapeAmount);
		while (shapeAmount--)
		{
			PxGeometryHolder geometryHolder = shapes[shapeAmount]->getGeometry();
			PxCapsuleGeometry capsuleGeometry = geometryHolder.capsule();
			PxTransform transform = link->getGlobalPose();
			
			mat4 rotation = glm::toMat4(quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
			Gizmos::AddCapsule(vec3(transform.p.x, transform.p.y, transform.p.z), capsuleGeometry.halfHeight * 2,
				capsuleGeometry.radius, 10, 10, vec4(1, 0, 0, 1), vec3(1, 0, 0), &rotation);
		}
		delete[] shapes;
	}
	delete[] links;

	return true;
}

// Actor
void PhysXEngine::AddActor(PhysicsObject* _actor) {
	GameObject* gameObject = _actor->gameObject;

	// Particle Emitter
	ParticleEmitter* particleEmitter = gameObject->GetComponent<ParticleEmitter>();
	if (particleEmitter) { 
		CreateParticleEmitter(*particleEmitter, _actor); 
	}

	// Rigidbody
	Rigidbody* rigidbody = gameObject->GetComponent<Rigidbody>();
	if (rigidbody) {
		CreateRigidbody(*rigidbody, _actor);
	}

	// Character Controller
	CharacterController* characterController = gameObject->GetComponent<CharacterController>();
	if (characterController) {
		CreateCharacterController(*characterController, _actor);
	}
}

bool PhysXEngine::RemoveActor(PhysicsObject* _actor) {
	for (unsigned int actorIndex = 0;
		actorIndex < actors.size();
		++actorIndex) {
		// Check if actor already exists in list 
		if (actors[actorIndex].physicsObject == _actor) {
			actors.erase(actors.begin() + actorIndex);
			return true;
		}
	}
	return false;
}

// Articulation
void PhysXEngine::AddArticulation(PhysicsObject* _articulation) {
	GameObject* gameObject = _articulation->gameObject;

	// Note(Manny): Needs to be an articulation not an actor...
	
	Ragdoll* ragdoll = gameObject->GetComponent<Ragdoll>();
	if (ragdoll) { 
		CreateRagdoll(*ragdoll, _articulation); 
	}
}

bool PhysXEngine::RemoveArticulation(PhysicsObject* _articulation) {
	for (unsigned int articulationIndex = 0;
		articulationIndex < articulations.size();
		++articulationIndex) {
		//Check if actor already exists in list 
		if (articulations[articulationIndex].physicsObject == _articulation) {
			articulations.erase(articulations.begin() + articulationIndex);
			return true;
		}
	}
	return false;
}

void PhysXEngine::SetupVisualDebugger() {
	// Check if PvdConnnection manager is available on this platform
	if (physics->getPvdConnectionManager() == 0) { 
		return; 
	}

	// Setup connection parameters
	const char* pvdHostIP = "127.0.0.1";

	// IP of the PC which is running PVD
	int port = 5425;

	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;

	// Timeout in milliseconds to wait for PVD to respond
	// NOTE: consoles and remote PCs need a higher timeout
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

	// Try to connectPxVisualDebuggerExt
	auto connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(),
		pvdHostIP, port, timeout, connectionFlags);
}

bool PhysXEngine::CreateParticleEmitter(ParticleEmitter& _particleEmitter, PhysicsObject* _actor) {
	PhysXActor particleEmitter(_actor);

	ParticleEmitter& pE = _particleEmitter;

	PxU32 maxParticles = pE.maxParticles;

	PxParticleFluid* particleFluid = physics->createParticleFluid(maxParticles);

	bool perParticleRestoffset = false;
	particleFluid = physics->createParticleFluid(maxParticles, perParticleRestoffset);
	
	// Particle emitter settings
	particleFluid->setRestParticleDistance(pE.restParticleDistance);
	particleFluid->setDynamicFriction(pE.dynamicFriction);
	particleFluid->setStaticFriction(pE.staticFriction);
	particleFluid->setDamping(pE.damping);
	particleFluid->setParticleMass(pE.particleMass);
	particleFluid->setRestitution(pE.restitution);
	particleFluid->setStiffness(pE.stiffness);

	// eCOLLISION_TWOWAY tells PhysX to sends a collision response to any object the particles hit
	particleFluid->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, false);
	
	if (particleFluid) {
		particleEmitter.pxActor = particleFluid;
		_particleEmitter.particleFluid = particleFluid;
		physicsScene->addActor(*particleFluid);
	}

	return true;
}

bool PhysXEngine::CreateRigidbody(Rigidbody& _rigidbody, PhysicsObject* _actor) {
	GameObject* gameObject = _actor->gameObject;
	vec3 position = gameObject->transform.position;
	quat rotation = gameObject->transform.rotation;
	PxTransform pose = PxTransform(PxVec3(position.x, position.y, position.z), PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

	// Stores the geometry for the collider
	PxGeometry* geometry = nullptr;
	vector<PxGeometry*> geometries;

	Collider* collider = gameObject->GetComponent<Collider>();
	if (collider != nullptr) {
		geometry = GetColliderGeometry(collider);

		// Add to PhysX Scene
		if (collider->shapeId != SHAPE_PLANE) {
			PhysXActor physicsObject(_actor);

			physicsObject.shapeId = collider->shapeId;
			if (collider->shapeId != SHAPE_MESH) {
				if (!gameObject->isStatic)
					physicsObject.pxActor = PxCreateDynamic(*physics, pose, *geometry, *physicsMaterial, _rigidbody.density);
				else
					physicsObject.pxActor = PxCreateStatic(*physics, pose, *geometry, *physicsMaterial);

				_rigidbody.isKinematic = false;
			} else {
				PxShape* newShape = physics->createShape(*geometry, *physicsMaterial);
				
				PxRigidDynamic* pxRigidDynamic = PxCreateKinematic(*physics, pose, *newShape, _rigidbody.density);

				_rigidbody.isKinematic = true;

				geometries = GetMeshColliderGeometry(collider);

				if (geometries.size() > 0) {
					for (unsigned int i = 0; i < geometries.size(); ++i) {
						newShape = physics->createShape(*geometries[i], *physicsMaterial);
						if (newShape) {
							pxRigidDynamic->attachShape(*newShape);
						}
					}
				}
				physicsObject.pxActor = pxRigidDynamic;
			}
			physicsScene->addActor(*physicsObject.pxActor);
			actors.push_back(physicsObject);
		}
	}
	return true;
}

bool PhysXEngine::CreateCharacterController(CharacterController& _characterController, PhysicsObject* _actor) {
	GameObject* gameObject = _actor->gameObject;
	vec3 position = gameObject->transform.position;
	quat rotation = gameObject->transform.rotation;
	PxTransform pose = PxTransform(PxVec3(position.x, position.y, position.z), 
								   PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));

	CharacterController& controller = _characterController;

	characterHitReport = new PhysXControllerHitReportCallback();

	// Create physics material
	playerPhysicsMaterial = physics->createMaterial(1.0f, 0.5f, 0.1f);

	PxCapsuleControllerDesc controllerDesc;
	controllerDesc.height = controller.height * 0.5f;
	controllerDesc.radius = controller.radius;
	controllerDesc.density = controller.density;
	controllerDesc.slopeLimit = controller.slopeLimit;
	controllerDesc.stepOffset = controller.stepOffset;
	controllerDesc.upDirection = PxVec3(0, 1, 0);
	controllerDesc.material = playerPhysicsMaterial;
	controllerDesc.reportCallback = characterHitReport;
	controllerDesc.position = PxExtendedVec3(0, 0, 0);

	playerController = characterManager->createController(controllerDesc);
	
	PhysXActor characterController(_actor);
	PxRigidDynamic* pxRigidDynamic = playerController->getActor();

	characterHitReport->clearPlayerContactNormal();

	characterController.pxActor = pxRigidDynamic;

	actors.push_back(characterController);

	return true;
}

bool PhysXEngine::CreateRagdoll(Ragdoll& _ragdoll, PhysicsObject* _articulation) {
	GameObject* gameObject = _articulation->gameObject;
	vec3 position = gameObject->transform.position;
	quat rotation = gameObject->transform.rotation;
	vec3 scale = gameObject->transform.scale;
	float scaleFactor = glm::max(scale.x, glm::max(scale.y, scale.z));
	PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z),
								   PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
	
	Ragdoll* ragdoll = gameObject->GetComponent<Ragdoll>();

	// Create the articulation for ragdoll
	PxArticulation* articulation = physics->createArticulation();
	RagdollNode** currentNode = ragdoll->ragdollData;

	// While there are more nodes to process
	while (*currentNode != nullptr) {
		// Get a pointer to the current node
		RagdollNode* currentNodePointer = *currentNode;
		// Create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		// Get scaled values for capsule
		float radius = currentNodePointer->radius * scaleFactor;
		float halfLength = currentNodePointer->halfLength * scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; // This will be set later if there is a parent

		PxArticulationLink* parentLinkPointer = nullptr;
		currentNodePointer->scaledGlobalPosition = transform.p;

		if (currentNodePointer->parentNodeIndex != -1) {
			// If there is a parent then the local position needs to be calculated for the link

			// Get a pointer to the parent node
			parentNode = *(ragdoll->ragdollData + currentNodePointer->parentNodeIndex);
			// Get a pointer to the link for the parent
			parentLinkPointer = parentNode->linkPointer;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) * scaleFactor;
			// Calculate local position of the node
			PxVec3 currentRelative = currentNodePointer->childLinkPosition *
				currentNodePointer->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));

			PxVec3 parentRelative = -currentNodePointer->parentLinkPosition *
				currentNodePointer->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));

			currentNodePointer->scaledGlobalPosition = parentNode->scaledGlobalPosition -
				(parentRelative + currentRelative);
		}

		// Build the transform for the link
		PxTransform linkTransform = PxTransform(currentNodePointer->scaledGlobalPosition,
			currentNodePointer->globalRotation);

		// Create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPointer, linkTransform);

		// Add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePointer->linkPointer = link;
		float jointSpace = 0.01f; // Gap between joints
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + 0.01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, * ragdollMaterial); // Adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f); // Adds some mass, mass should really be part of the data!

		if (currentNodePointer->parentNodeIndex != -1) {
			// Get the pointer to the joint from the link
			PxArticulationJoint* joint = link->getInboundJoint();
			// Get the relative rotation of this linl
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() *
				currentNodePointer->globalRotation;
			// Set the parent constraint frame
			PxTransform parentConstraintFrame =
				PxTransform(PxVec3(currentNodePointer->parentLinkPosition * parentHalfLength, 0, 0),
				frameRotation);

			// Set the child constraint frame (this is the constraing frame of the newly added link)
			PxTransform thisConstraintFrame =
				PxTransform(PxVec3(currentNodePointer->childLinkPosition * childHalfLength, 0, 0));

			// Set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			// Set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}

		currentNode++;
	}

	PhysXArticulation ragdollArticulation(_articulation);

	ragdollArticulation.pxArticulation = articulation;

	physicsScene->addArticulation(*articulation);
	articulations.push_back(ragdollArticulation);

	return true;
}

PxGeometry* PhysXEngine::GetColliderGeometry(Collider* _collider) {
	if (_collider == nullptr) {
		Debug::LogError("PhysX error. Collider cannot be null. Line '" + to_string(__LINE__));
		return nullptr;
	}

	PxGeometry* geometry = nullptr;
	switch (_collider->shapeId) {
	case SHAPE_PLANE: {
		PlaneCollider* plane = dynamic_cast<PlaneCollider*>(_collider);
		geometry = new PxPlaneGeometry();
	} break;
	case SHAPE_SPHERE: {
		SphereCollider* sphere = dynamic_cast<SphereCollider*>(_collider);
		geometry = new PxSphereGeometry(sphere->radius);
	} break;
	case SHAPE_BOX: {
		BoxCollider* box = dynamic_cast<BoxCollider*>(_collider);
		geometry = new PxBoxGeometry(box->obb.e.x, box->obb.e.y, box->obb.e.z);
	} break;
	case SHAPE_CAPSULE: {
		CapsuleCollider* capsule = dynamic_cast<CapsuleCollider*>(_collider);
		geometry = new PxCapsuleGeometry(capsule->radius, capsule->height * 0.5f);
	} break;
	case SHAPE_MESH: {
		MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(_collider);
		GameObject* gameObject = meshCollider->gameObject;
		Mesh* meshObject = meshCollider->meshObject;
		IndexedModel* model = meshObject->model;

		MeshData& mesh = model->meshes[0];

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = mesh.positions.size();
		meshDesc.points.stride = sizeof(vec3);
		meshDesc.points.data = mesh.positions.data();

		meshDesc.triangles.count = mesh.indices.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(unsigned int);
		meshDesc.triangles.data = mesh.indices.data();

		PxDefaultMemoryOutputStream writeBuffer;
		bool status = physicsCooker->cookTriangleMesh(meshDesc, writeBuffer);

		if (status) {
			PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

			PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

			PxMeshScale scale(PxVec3(glm::max(0.01f, gameObject->transform.scale.x),
				glm::max(0.01f, gameObject->transform.scale.y),
				glm::max(0.01f, gameObject->transform.scale.z)), PxQuat(1.0f));

			geometry = new PxTriangleMeshGeometry(triangleMesh, scale);
		}
		else
			Debug::LogWarning("PxCooker failed!");

	} break; // Handled elsewhere
	default:
		std::cout << "Error: ShapeID is not valid!" << std::endl;
	}
	
	return geometry;
}

vector<PxGeometry*> PhysXEngine::GetMeshColliderGeometry(Collider* _collider) {
	vector<PxGeometry*> geometries;

	MeshCollider* meshCollider = dynamic_cast<MeshCollider*>(_collider);
	if (meshCollider) {
		GameObject* gameObject = meshCollider->gameObject;
		Mesh* meshObject = meshCollider->meshObject;
		IndexedModel* model = meshObject->model;

		for (unsigned int i = 0; i < model->meshes.size(); ++i) {
			MeshData& mesh = model->meshes[i];

			PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = mesh.positions.size();
			meshDesc.points.stride = sizeof(vec3);
			meshDesc.points.data = mesh.positions.data();

			meshDesc.triangles.count = mesh.indices.size() / 3;
			meshDesc.triangles.stride = 3 * sizeof(unsigned int);
			meshDesc.triangles.data = mesh.indices.data();

			PxDefaultMemoryOutputStream writeBuffer;
			bool status = physicsCooker->cookTriangleMesh(meshDesc, writeBuffer);
			
			if (status) {
				PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

				PxTriangleMesh* triangleMesh = physics->createTriangleMesh(readBuffer);

				PxMeshScale scale(PxVec3(glm::max(0.01f, gameObject->transform.scale.x),
					glm::max(0.01f, gameObject->transform.scale.y),
					glm::max(0.01f, gameObject->transform.scale.z)), PxQuat(1.0f));

				geometries.push_back(new PxTriangleMeshGeometry(triangleMesh, scale));
			} else {
				Debug::LogWarning("PxCooker failed!");
			}
		}
	}

	return geometries;
}