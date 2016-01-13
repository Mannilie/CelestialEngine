#include "Scene.h"

// Components
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "PlaneCollider.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "Lighting.h"
#include "Ragdoll.h"
#include "Fluid.h"
#include "ParticleEmitter.h"
#include "CharacterController.h"

// Scripts 
#include "FlyCameraScript.h"

// Physics
#include "PhysXEngine.h"

// Debugging
#include "Gizmos.h"

Scene::Scene(){}
Scene::~Scene() {
	for (unsigned int i = 0; i < objects.size(); ++i) {
		delete objects[i];
	}
}

void Scene::Init(CoreEngine* _engine) {
	_engine->physicsEnabled = true; // Enable engine physics upon startup
	renderer = _engine->renderer;

	// Create default Materials
	Material defaultTexture("default_texture", Texture("default_texture.png"), NULL, 0.0f, 20.0f, NULL, NULL, 0.03f, -0.5f);

	// Set up main camera
	GameObject* mainCamera = new GameObject("Main Camera");
	mainCamera->transform.isSelectable = false;
	mainCamera->transform.position = vec3(10, 15, 15);
	mainCamera->transform.LookAt(vec3(0));
	Camera::current = mainCamera->AddComponent<Camera>(Camera((float)Window::width / (float)Window::height, 75.0f));
	mainCamera->AddComponent<FlyCameraScript>();
	AddToScene(mainCamera);

	// Set up directional light
	GameObject* directionalLight = new GameObject("Directional Light");
	directionalLight->transform.isSelectable = false;
	directionalLight->transform.position = vec3(0,0,0);
	directionalLight->AddComponent<DirectionalLight>();
	AddToScene(directionalLight);
}

bool Scene::Update() {
	if (Game::Update() == false) {
		return false;
	}
	return true;
}
void Scene::Draw(RenderingEngine* _renderer) {
	renderer = _renderer;
	renderer->Render(objects);
}
