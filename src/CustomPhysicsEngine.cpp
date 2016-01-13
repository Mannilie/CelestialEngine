#include "CustomPhysicsEngine.h"

// Object
#include "GameObject.h"

// Physics
#include "PhysicsObject.h"

// Components
#include "Rigidbody.h"
#include "Collider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "PlaneCollider.h"

// Debugging
#include "Gizmos.h"
#include "Debug.h"

// Utilities
#include "Time.h"

float squared(float _n) { return _n * _n; }
typedef CollisionManifold(*CollisionFunction)(Collider*, Collider*);
static CollisionFunction CollisionFunctionTable[] = {
	0,									CustomPhysicsEngine::PlaneToSphere,		CustomPhysicsEngine::PlaneToBox,
	CustomPhysicsEngine::SphereToPlane, CustomPhysicsEngine::SphereToSphere,	CustomPhysicsEngine::SphereToBox,
	CustomPhysicsEngine::BoxToPlane,	CustomPhysicsEngine::BoxToSphere,		CustomPhysicsEngine::BoxToBox
};
void CustomPhysicsEngine::Shutdown() {
	actors.clear();
}
bool CustomPhysicsEngine::Update() {
	for (unsigned int actorIndex = 0; 
		actorIndex < actors.size(); 
		++actorIndex) {
		actors[actorIndex]->PhysicsUpdate(Time::deltaTime > 0.033f ? 0.033f : Time::deltaTime);
	}

	if (collisionEnabled) {
		CheckForCollisions();
	}

	return true;
}
void CustomPhysicsEngine::AddActor(PhysicsObject* _actor) {
	actors.push_back(_actor);
}
bool CustomPhysicsEngine::RemoveActor(PhysicsObject* _actor) {
	for (unsigned int actorIndex = 0;
		actorIndex < actors.size();
		++actorIndex)
	{
		if (actors[actorIndex] == _actor) {
			actors.erase(actors.begin() + actorIndex);
			return true;
		}
	}
	return false;
}
void CustomPhysicsEngine::CheckForCollisions() {
	int actorCount = actors.size();
	//Loop through all actors twice
	for (int firstActor = 0;
		firstActor < actorCount - 1;
		++firstActor) {
		for (int secondActor = firstActor + 1;
			secondActor < actorCount;
			++secondActor) {
			PhysicsObject* actorA = actors[firstActor];
			PhysicsObject* actorB = actors[secondActor];

			if (actorA->gameObject->colliders.size() == 0 ||
				actorB->gameObject->colliders.size() == 0) {
				continue;
			}

			Collider* colliderA = actorA->gameObject->colliders[0];
			Collider* colliderB = actorB->gameObject->colliders[0];

			int shapeId1 = colliderA->shapeId;
			int shapeId2 = colliderB->shapeId;

			//SHAPE_COUNT - 2 for capsule AND mesh
			int index = (shapeId1 * (SHAPE_COUNT)) + shapeId2;

			CollisionFunction collisionFunction = CollisionFunctionTable[index];
			if (collisionFunction != nullptr) {
				CollisionManifold manifold = collisionFunction(colliderA, colliderB);
				if (manifold.isColliding) {
					//Add collision response here
					Rigidbody* rigidA = colliderA->attachedRigidbody;
					Rigidbody* rigidB = colliderB->attachedRigidbody;

					if (rigidA == nullptr || rigidB == nullptr) {
						continue;
					}

					if (!rigidA->gameObject->isStatic) {
						rigidA->transform->position += manifold.intersectionA * manifold.normal;
					}

					if (!rigidB->gameObject->isStatic) {
						rigidB->transform->position += manifold.intersectionB * manifold.normal;
					}

					ApplyCollisionResolution(&manifold, rigidA);
					ApplyCollisionResolution(&manifold, rigidB);
				}
			}
		}
	}
}
void CustomPhysicsEngine::ApplyCollisionResolution(CollisionManifold* _manifold, Rigidbody* _rigid) {
	//Get Mass
	float mass = _rigid->mass;

	//Get Moment Of Inertia
	float moi = _rigid->momentOfInertia;

	//Get Inverse Mass 
	float invMass = 1.0f / _rigid->mass;

	//Get Inverse Moment Of Inertia
	float invMOI = 1.0f / _rigid->momentOfInertia;
	
	vec3 relativePoint = _manifold->point - _rigid->transform->position;
	vec3 velocity = _rigid->velocity + _rigid->angularVelocity * relativePoint;

	vec3 rXn = glm::cross(relativePoint, _manifold->normal) * invMOI;
	
	float rXnDn = glm::dot(rXn, _manifold->normal);
	
	float numerator = -(1.0f + _manifold->restitution) * glm::dot(velocity, _manifold->normal);
	float denominator = (1.0f * invMass) + rXnDn;

	float j = numerator / denominator;
	vec3 impulse = _manifold->normal * j;
	if (!_rigid->gameObject->isStatic) {
		_rigid->velocity = velocity + impulse * invMass;
		_rigid->angularVelocity += glm::cross(impulse, relativePoint) * invMOI;
	}

}
void CustomPhysicsEngine::ApplyCollisionResolutionTest(CollisionManifold* _manifold, Rigidbody* _rigidA, Rigidbody* _rigidB) {
	//Get Mass
	float massA = _rigidA->mass;
	float massB = _rigidB->mass;

	//Get Moment Of Inertia
	float moiA = _rigidA->momentOfInertia;
	float moiB = _rigidB->momentOfInertia;

	//Get Inverse Mass 
	float invMassA = 1.0f / _rigidA->mass;
	float invMassB = 1.0f / _rigidB->mass;

	//Get Inverse Moment Of Inertia
	float invMOIA = 1.0f / _rigidA->momentOfInertia;
	float invMOIB = 1.0f / _rigidB->momentOfInertia;

	vec3 relativePointA = _manifold->point - _rigidA->transform->position;
	vec3 relativePointB = _manifold->point - _rigidB->transform->position;
	
	vec3 rXn_A = glm::cross(relativePointA, _manifold->normal) / moiA;
	vec3 rXn_B = glm::cross(relativePointB, _manifold->normal) / moiB;

	vec3 velocityA = _rigidA->velocity + _rigidA->angularVelocity * relativePointA;
	vec3 velocityB = _rigidB->velocity + _rigidB->angularVelocity * relativePointB;

	vec3 relativeVelocity = velocityA - velocityB;

	float rXnDn_A = glm::dot(rXn_A, _manifold->normal);
	float rXnDn_B = glm::dot(rXn_B, _manifold->normal);

	float numerator = -(1.0f + _manifold->restitution) * glm::dot(relativeVelocity, _manifold->normal);

	float denominator = glm::dot(_manifold->normal, _manifold->normal * (invMassA + invMassB)) +
						squared(rXnDn_A) / moiA + 
						squared(rXnDn_B) / moiB;
	
	float j = numerator / denominator;

	vec3 impulse = _manifold->normal * j;
	if (!_rigidA->gameObject->isStatic) {
		_rigidA->velocity += (impulse * invMassA);
		_rigidA->angularVelocity += glm::cross(rXn_A, impulse) / moiA;
	}

	if (!_rigidB->gameObject->isStatic)	{	
		_rigidB->velocity += (-impulse * invMassB);
		_rigidB->angularVelocity += glm::cross(rXn_B, -impulse) / moiB;
	}
}
CollisionManifold CustomPhysicsEngine::SphereToSphere(Collider* _first, Collider* _second) {
	CollisionManifold result = {};

	SphereCollider* firstSphere = (SphereCollider*)_first;
	SphereCollider* secondSphere = (SphereCollider*)_second;

	// Get the vector from the first to the second sphere
	vec3 delta = secondSphere->transform->position - firstSphere->transform->position;
	// The length of the delta is the distance
	float distance = glm::length(delta);
	float radiusSum = firstSphere->radius + secondSphere->radius;

	result.colliderA = firstSphere;
	result.colliderB = secondSphere;
	result.isColliding = false;

	if (distance <= radiusSum) {
		// Get the normal of the collision
		vec3 collisionNormal = glm::normalize(delta);
		result.intersectionA = -(radiusSum - distance);
		result.intersectionB = (radiusSum - distance);
		result.restitution = 0.5f;
		result.normal = collisionNormal;
		result.point = firstSphere->transform->position + collisionNormal * firstSphere->radius;
		result.isColliding = true;
	}

	return result;
}
CollisionManifold CustomPhysicsEngine::SphereToPlane(Collider* _sphere, Collider* _plane) {
	CollisionManifold result = {};

	SphereCollider* sphere = (SphereCollider*)_sphere;
	PlaneCollider* plane = (PlaneCollider*)_plane;

	float perpDistance = glm::dot(sphere->transform->position, plane->normal) - plane->distance;

	result.colliderA = sphere;
	result.colliderB = plane;
	result.isColliding = false;

	if (perpDistance <= sphere->radius) {
		result.isColliding = true;
		result.intersectionA = sphere->radius - perpDistance;
		result.restitution = 0.5f;
		result.normal = plane->normal;
		result.point = sphere->transform->position - plane->normal * sphere->radius;
	}

	return result;
}
CollisionManifold CustomPhysicsEngine::PlaneToSphere(Collider* _plane, Collider* _sphere) {
	return SphereToPlane(_sphere, _plane);
}
CollisionManifold CustomPhysicsEngine::BoxToBox(Collider* _first, Collider* _second) {
	CollisionManifold result = {};

	BoxCollider* BoxA = (BoxCollider*)_first;
	BoxCollider* BoxB = (BoxCollider*)_second;

	result.isColliding = false;
	result.colliderA = BoxA;
	result.colliderB = BoxB;
	result.restitution = 0.5f;

	Manifold manifold;
	if (BoxA->obb.Intersects(BoxB->obb, &manifold)) {
		result.isColliding = true;
		result.intersectionA = manifold.contacts->penetration;
		result.intersectionB = -manifold.contacts->penetration;
		result.normal = manifold.normal;
		
		return result;
	}

	return result;
}
CollisionManifold CustomPhysicsEngine::BoxToPlane(Collider* _box, Collider* _plane) {
	CollisionManifold result = {};

	// Note(Manny): Implement box to plane when possible.
	
	return result;
}
CollisionManifold CustomPhysicsEngine::PlaneToBox(Collider* _plane, Collider* _box) {
	return BoxToPlane(_box, _plane);
}
CollisionManifold CustomPhysicsEngine::SphereToBox(Collider* _sphere, Collider* _box) {
	return BoxToSphere(_box, _sphere);
}
CollisionManifold CustomPhysicsEngine::BoxToSphere(Collider* _box, Collider* _sphere) {
	CollisionManifold result = {};

	BoxCollider* box = (BoxCollider*)_box;
	SphereCollider* sphere = (SphereCollider*)_sphere;

	vec3 center = sphere->transform->position;

	vec3 min = box->bounds.min;
	vec3 max = box->bounds.max;

	float radius = sphere->radius;

	result.colliderA = box;
	result.colliderB = sphere;
	result.isColliding = false;

	vec3 closestPoint;
	closestPoint.x = (center.x < min.x) ? min.x : (center.x > max.x) ? max.x : center.x;
	closestPoint.y = (center.y < min.y) ? min.y : (center.y > max.y) ? max.y : center.y;
	closestPoint.z = (center.z < min.z) ? min.z : (center.z > max.z) ? max.z : center.z;
	
	vec3 direction = glm::normalize(center - closestPoint);
	float distance = glm::length(center - closestPoint);

	if (distance <= radius) {
		// Collision response goes here
		result.restitution = 0.75f;
		result.isColliding = true;
		result.intersectionB = (radius - distance);
		result.normal = glm::normalize(center - closestPoint);
		result.point = closestPoint;
	}

	return result;
}