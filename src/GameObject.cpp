#include "GameObject.h"

// Components
#include "Component.h"
#include "MeshRenderer.h"
#include "Lighting.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "Rigidbody.h"
#include "ParticleEmitter.h"

// GUI
#include "imgui.h"

// Structs
#include "Ray.h"

// Other
using std::to_string;

GameObject::GameObject(string _name) {
	if (_name == "GameObject") {
		_name = "GameObject " + to_string(instanceID);
	}
	name = _name;
	isVisible = true;
	transform.gameObject = this;
}
bool GameObject::Startup() {
	return true;
}
void GameObject::Shutdown() {
	for (unsigned int i = 0; i < components.size(); ++i) {
		delete components[i];
	}
	components.clear();
}
bool GameObject::Update() {
	for (unsigned int componentIndex = 0;
		componentIndex < components.size();
		++componentIndex) {
		components[componentIndex]->Update();
	}
	return true;
}
void GameObject::Draw(RenderingEngine& _renderer) {
	for (unsigned int componentIndex = 0;
		componentIndex < components.size();
		++componentIndex) {
		components[componentIndex]->Draw(_renderer);
	}
}
Collider* GameObject::AddCollider(Collider* _collider) {
	colliders.push_back(_collider);
	return _collider;
}