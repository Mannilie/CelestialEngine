#include "CoreEngine.h"

#include "RenderingEngine.h"
#include "Game.h"
#include "Input.h"
#include "Time.h"
#include "Gizmos.h"
#include "Debug.h"
#include "GUI.h"

PhysicsEngine* CoreEngine::physics = nullptr;

CoreEngine::CoreEngine(RenderingEngine* _renderer, PhysicsEngine* _physics, Game* _game) :
	renderer(_renderer),
	game(_game),
	physicsEnabled(true) {
	physics = _physics;
}

bool CoreEngine::Startup() {
	Time::Create();
	Input::Create();
	if (physics->Startup() == false) {
		return false;
	}
	game->Init(this);
	Gizmos::Create();
	GUI::Create();
	return true;
}

void CoreEngine::Shutdown() {
	Window::Shutdown();
	Gizmos::Destroy();
	Input::Shutdown();
	Texture::Shutdown();
	Material::Shutdown();
	Shader::Shutdown();
	physics->Shutdown();
}

bool CoreEngine::Update() {
	Time::Update();
	if (Window::IsCloseRequested()) {
		return false;
	}
	Window::Update();
	GUI::Update();
	Debug::Update();
	if (Debug::HasError()) {
		return true;
	}
	Gizmos::Clear();
	Input::Update();
	Transform::UpdateTransformSelection();
	if (physicsEnabled) {
		physics->Update();
	}
	game->Update();
	return true;
}

void CoreEngine::Draw() {
	// Clear OpenGL buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	// Render the game scene
	game->Draw(renderer);
	GUI::Draw();
	glDisable(GL_DEPTH_TEST);
	ImGui::Render();
	glEnable(GL_DEPTH_TEST);
	Window::Draw();
}