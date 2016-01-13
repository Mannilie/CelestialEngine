
#include "MeshCollider.h"

// Object
#include "GameObject.h"

// Components
#include "MeshRenderer.h"
#include "Rigidbody.h"

// Structs
#include "Mesh.h"
#include "LineSegment.h"

// GUI
#include "GUI.h"

// Debugging
#include "Gizmos.h"

MeshCollider::MeshCollider() : meshObject(nullptr) { shapeId = SHAPE_MESH; }
MeshCollider::~MeshCollider(){}

bool MeshCollider::Startup() {
	gameObject->AddCollider(this);

	MeshRenderer* renderer = this->gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = this->transform->position;
		bounds.size = this->transform->scale * meshBounds.size;
		bounds.min = bounds.center - bounds.size;
		bounds.max = bounds.center + bounds.size;
		meshObject = &renderer->mesh;
	}
	
	return true;
}

bool MeshCollider::Update() {
	if (this->transform && this->transform->isSelected) { Inspector(); }

	attachedRigidbody = this->gameObject->GetComponent<Rigidbody>();

	MeshRenderer* renderer = this->gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = this->transform->position;
		bounds.size = this->transform->scale * meshBounds.size;
		bounds.min = bounds.center - bounds.size;
		bounds.max = bounds.center + bounds.size;
	}
	
	return true;
}

bool MeshCollider::Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance) {
	return false;
}

void MeshCollider::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("MeshCollider")) {
		// Settings go here
		ImGui::Checkbox("Enabled", &enabled);
		ImGui::Checkbox("isTrigger", &isTrigger);
		ImGui::DragFloat3("Center", (float*)&bounds.center);
		ImGui::DragFloat3("Size", (float*)&bounds.size);
		
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}