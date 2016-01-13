#include "Bounds.h"
#include "Gizmos.h"
#include "GUI.h"

#include "Ray.h"

Bounds::Bounds() : 
	center(vec3(0)),
	extents(vec3(0)),
	size(1),
	min(vec3(0)),
	max(vec3(1)),
	initializedOldStuff(false) {}
Bounds::Bounds(vec3 _center, vec3 _size) :
	center(_center),
	extents(vec3(0)),
	size(_size),
	min(vec3(0)),
	max(vec3(1)) {}
Bounds::~Bounds() {}
bool Bounds::Startup() { return true; }
bool Bounds::Update() { return true; }
void Bounds::Draw() {
	Gizmos::AddAABB(this->center, this->extents, vec4(1, 1, 0, 1));
	Gizmos::AddAABB(this->center, this->size, vec4(1, 0, 1, 1));

	Gizmos::AddSphere(this->min, 0.5f, 4, 4, vec4(0, 1, 0, 1));
	Gizmos::AddSphere(this->max, 0.5f, 4, 4, vec4(0, 1, 0, 1));
}
bool Bounds::IntersectRay(Ray& _ray) {
	Bounds& bounds = *this;

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

	return (tmax >= tmin);
}
bool Bounds::RayCast(Ray& _ray, RaycastHit* _hitInfo, float _maxDistance) {
	Bounds& bounds = *this;

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

	_hitInfo->point = hitPoint;
	_hitInfo->distance = distance;
	_hitInfo->collider = nullptr;
	_hitInfo->transform = nullptr;

	return (tmax >= tmin);
}
bool Bounds::Intersects(Bounds& _bounds) {
	return false;
}
void Bounds::SetMinMax(vec3 _min, vec3 _max) {
	min = _min;
	max = _max;
	center = (min + max) * 0.5f;
	size = (max - min) * 0.5f;
}
vec3 Bounds::operator[](int _index) {
	if (_index == 0)
		return min;
	if (_index == 1)
		return max;

	return vec3(-1);
}
void Bounds::Inspector() {
	if (ImGui::TreeNode("Bounds")) {
		ImGui::DragFloat3("Center", (float*)&center);
		ImGui::DragFloat3("Extents", (float*)&extents);
		ImGui::DragFloat3("Size", (float*)&size);
		ImGui::TreePop();
	}
}
bool Bounds::HasChanged() {
	// Check if bounds members have changes
	if (oldMin != min) { return true; }
	if (oldMax != max) { return true; }
	if (oldExtents != extents) { return true; }
	if (oldSize != size) { return true; }
	return false;
}