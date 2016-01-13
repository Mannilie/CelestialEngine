#include "BoxCollider.h"

#include "GameObject.h"
#include "Gizmos.h"
#include "GUI.h"
#include "MeshRenderer.h"
#include "Mesh.h"

#include "Rigidbody.h"
#include "LineSegment.h"

BoxCollider::BoxCollider() { shapeId = SHAPE_BOX; }
BoxCollider::~BoxCollider() {}

bool BoxCollider::Startup() {
	gameObject->AddCollider(this);

	MeshRenderer* renderer = this->gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = this->transform->position;
		bounds.size = this->transform->scale * meshBounds.size;
		bounds.min = bounds.center - bounds.size;
		bounds.max = bounds.center + bounds.size;
	}
	obb.SetFrom(bounds, *this->transform);

	return true;
}

bool BoxCollider::Update() {
	if (this->transform && this->transform->isSelected) Inspector();
	
	attachedRigidbody = this->gameObject->GetComponent<Rigidbody>();

	MeshRenderer* renderer = this->gameObject->GetComponent<MeshRenderer>();
	if (renderer != nullptr) {
		Bounds meshBounds = renderer->mesh.bounds;
		bounds.center = this->transform->position;
		bounds.size = this->transform->scale * meshBounds.size;
		bounds.min = bounds.center - bounds.size;
		bounds.max = bounds.center + bounds.size;
	}

	for (int i = 0; i < 12; ++i) {
		LineSegment line = obb.Edge(i);
		Gizmos::AddLine(line.start, line.end, vec4(0, 1, 0, 1));
	}

	obb.SetFrom(bounds, *this->transform);

	return true;
}

bool BoxCollider::Raycast(Ray _ray, RaycastHit& _hitInfo, float _maxDistance) {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin = (bounds[_ray.sign[0]].x - _ray.origin.x) * _ray.invDirection.x;
	tmax = (bounds[1 - _ray.sign[0]].x - _ray.origin.x) * _ray.invDirection.x;
	tymin = (bounds[_ray.sign[1]].y - _ray.origin.y) * _ray.invDirection.y;
	tymax = (bounds[1 - _ray.sign[1]].y - _ray.origin.y) * _ray.invDirection.y;
	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[_ray.sign[2]].z - _ray.origin.z) * _ray.invDirection.z;
	tzmax = (bounds[1 - _ray.sign[2]].z - _ray.origin.z) * _ray.invDirection.z;
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	vec3 hitPoint = _ray.origin + _ray.direction * tmin;
	float distance = glm::length(hitPoint - _ray.origin);
	
	if (distance > _maxDistance)
		return false;

	_hitInfo.point = hitPoint;
	_hitInfo.distance = distance;
	_hitInfo.collider = this;
	_hitInfo.transform = this->transform;

	return ((tmin < _maxDistance) && (tmax >= glm::max(0.0f, tmin)));
}

void BoxCollider::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("BoxCollider")) {
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