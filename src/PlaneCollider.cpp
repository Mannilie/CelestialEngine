#include "PlaneCollider.h"

// Object
#include "GameObject.h"

// Components
#include "MeshRenderer.h"
#include "Rigidbody.h"

// GUI
#include "GUI.h"

// Debugging
#include "Gizmos.h"

PlaneCollider::PlaneCollider() :
	normal(vec3(0)),
	distance(0) { shapeId = SHAPE_PLANE; }
PlaneCollider::~PlaneCollider(){}

bool PlaneCollider::Startup() {
	gameObject->AddCollider(this);
	return true;
}

bool PlaneCollider::Update() {
	if (transform && transform->isSelected) { Inspector(); }

	attachedRigidbody = gameObject->GetComponent<Rigidbody>();

	MeshRenderer* renderer = gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		bounds.center = renderer->mesh.bounds.center;
	} else {
		bounds.center = transform->position;
	}

	normal = transform->up;
	distance = glm::length(transform->position);
	return true;
}

void PlaneCollider::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("PlaneCollider")) {
		// Settings go here
		ImGui::Checkbox("Enabled", &enabled);
		ImGui::Checkbox("isTrigger", &isTrigger);
		ImGui::DragFloat3("Normal", (float*)&normal);
		ImGui::DragFloat("Distance", &distance);
		
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}