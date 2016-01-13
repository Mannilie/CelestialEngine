#include "SphereCollider.h"

// Object
#include "GameObject.h"

// Components
#include "Rigidbody.h"
#include "MeshRenderer.h"

// GUI
#include "GUI.h"

// Debugging
#include "Gizmos.h"

SphereCollider::SphereCollider() : 
radius(10.0f) { shapeId = SHAPE_SPHERE; }
SphereCollider::~SphereCollider(){}

bool SphereCollider::Startup() {
	gameObject->AddCollider(this);

	MeshRenderer* renderer = this->gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = this->transform->position;
		bounds.size = this->transform->scale * meshBounds.size;
		radius = glm::max(glm::max(bounds.size.x, bounds.size.y), bounds.size.z);
	}

	return true;
}

bool SphereCollider::Update() {
	if (transform && transform->isSelected) { Inspector(); }

	attachedRigidbody = gameObject->GetComponent<Rigidbody>();
	
	MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = transform->position;
		bounds.size = transform->scale * meshBounds.size;
		radius = glm::max(glm::max(bounds.size.x, bounds.size.y), bounds.size.z);
	}

	return true;
}

void SphereCollider::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("SphereCollider")) {
		// Settings go here
		ImGui::Checkbox("Enabled", &enabled);
		ImGui::Checkbox("isTrigger", &isTrigger);
		ImGui::DragFloat3("Center", (float*)&center);
		ImGui::DragFloat("Radius", &radius);
		
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}