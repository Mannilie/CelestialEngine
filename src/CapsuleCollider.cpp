#include "CapsuleCollider.h"
#include "GameObject.h"
#include "GUI.h"

#include "Rigidbody.h"

#include "Gizmos.h"

CapsuleCollider::CapsuleCollider() : 
	height(20.0f), 
	radius(5.0f) { shapeId = SHAPE_CAPSULE; }
CapsuleCollider::~CapsuleCollider(){}

bool CapsuleCollider::Startup() { 
	gameObject->AddCollider(this);
	return true; 
}

bool CapsuleCollider::Update() {
	if (this->transform && this->transform->isSelected) Inspector();
	
	attachedRigidbody = this->gameObject->GetComponent<Rigidbody>();

	bounds.center = this->transform->position;
	bounds.size = this->transform->scale * vec3(radius);

	Gizmos::AddCapsule(this->transform->position, height, radius, 16, 16, vec4(1, 0, 0, 1), vec3(0, 1, 0), &glm::toMat4(this->transform->rotation));

	return true;
}

//bool True when the ray intersects any collider, otherwise false.
bool CapsuleCollider::Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance) {
	return false;
}

void CapsuleCollider::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("CapsuleCollider")) {
		// Settings go here
		ImGui::Checkbox("Enabled", &enabled);
		ImGui::Checkbox("isTrigger", &isTrigger);
		ImGui::DragFloat("Radius", &radius);
		ImGui::DragFloat("Height", &height);
		
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}