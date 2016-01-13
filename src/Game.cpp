#include "Game.h"

// Components
#include "MeshRenderer.h"

// Structs
#include "Mesh.h"
#include "Material.h"

// Utilities
#include "imgui.h"
#include "Explorer.h"

bool Game::Update() {
	UpdateGUIElements();
	UpdateHierarchy();
	return true;
}

void Game::UpdateHierarchy() {
	ImGui::Begin("Hierarchy");

	for (unsigned int i = 0; i < objects.size(); ++i) {
		GameObject* gameObject = objects[i];
		Transform* transform = &gameObject->transform;

		if (!transform->parent) {
			if (ImGui::Selectable(objects[i]->name.c_str())) {
				Transform::DeselectAllTransforms();
				objects[i]->transform.isSelected = true;
			}
			transform->Update();
			gameObject->Update();
			UpdateChildren(transform);
		}
	}

	ImGui::End();
}
void Game::UpdateChildren(Transform* _transform) {
	ImGui::Indent();

	vector<Transform*>& children = _transform->children;
	for (unsigned int i = 0; i < children.size(); ++i) {
		GameObject* gameObject = children[i]->gameObject;
		Transform* transform = &gameObject->transform;
		
		if (ImGui::Selectable(gameObject->name.c_str())) {
			Transform::DeselectAllTransforms();
			transform->isSelected = true;
		}

		transform->Update();

		if (transform->parent) {
			transform->worldMatrix = transform->parent->worldMatrix * transform->worldMatrix;
		}

		gameObject->Update();
		
		if (transform->children.size() > 0) {
			UpdateChildren(transform);
		}

	}
	ImGui::Unindent();
}
void Game::UpdateGUIElements() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open...")) {
				string fileToOpen = Explorer::OpenFileDialog();
				if (fileToOpen.size() > 0) {
					GameObject* newMesh = new GameObject();
					newMesh->AddComponent<MeshRenderer>(MeshRenderer(Mesh(fileToOpen, false), Material("default_texture")));
					AddToScene(newMesh);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create")) {
			if (ImGui::MenuItem("Sphere")) {
				GameObject* sphere = new GameObject("Sphere");
				sphere->AddComponent<MeshRenderer>(MeshRenderer(Mesh("sphere.obj"), Material("default_texture")));
				AddToScene(sphere);
				Transform::SetSelectedTransform(&sphere->transform);
			}
			if (ImGui::MenuItem("Plane")) {
				GameObject* plane = new GameObject("Plane");
				plane->AddComponent<MeshRenderer>(MeshRenderer(Mesh("plane.obj"), Material("default_texture")));
				AddToScene(plane);
				Transform::SetSelectedTransform(&plane->transform);
			}
			if (ImGui::MenuItem("Cube")) {
				GameObject* cube = new GameObject("Cube");
				cube->AddComponent<MeshRenderer>(MeshRenderer(Mesh("cube.obj"), Material("default_texture")));
				AddToScene(cube);
				Transform::SetSelectedTransform(&cube->transform);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
void Game::AddToScene(GameObject* _gameObject) {
	// Starts up all of the game object components
	for (unsigned int i = 0; i < _gameObject->components.size(); ++i) {
		_gameObject->components[i]->Startup();
	}
	objects.push_back(_gameObject);
}
