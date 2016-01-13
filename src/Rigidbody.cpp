#include "Rigidbody.h"

// Sub-engines
#include "CoreEngine.h"
#include "PhysicsEngine.h"

// Objects
#include "GameObject.h"

// Components
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

// GUI
#include "GUI.h"

// Utilities
#include "Time.h"

Rigidbody::Rigidbody() : 
	mass(1.0f), 
	drag(0.0f),
	density(10.0f),
	angularDrag(0.5f),
	angularVelocity(vec3(0)),
	angularMomentum(vec3(0)),
	useGravity(false),
	isKinematic(false),
	staticFriction(0.1f),
	dynamicFriction(0.5f),
	isChangedInGUI(false)
{}
Rigidbody::~Rigidbody(){}
bool Rigidbody::Startup() {
	CoreEngine::physics->AddActor(this);
	return true;
}
void Rigidbody::Shutdown() {
	CoreEngine::physics->RemoveActor(this);
}
bool Rigidbody::Update() {
	if (transform && transform->isSelected) Inspector();
	return true;
}
void Rigidbody::PhysicsUpdate(float _timeStep) {
	if (gameObject->isStatic) {
		return;
	}
	CalculateMomentOfInertia();
	AddForce( (useGravity ? CoreEngine::physics->gravity * mass : vec3(0)));
	vec3 frictionForce = -mass * dynamicFriction * velocity;
	AddForce(frictionForce);
	vec3 frictionTorque = (-momentOfInertia * dynamicFriction * angularVelocity);
	AddTorque(frictionTorque);
	vec3 acceleration = totalForce / mass;
	vec3 angularAcceleration = totalTorque / momentOfInertia;
	OldPosition = transform->position; //keep our old position for collision response
	velocity += acceleration * _timeStep;
	transform->position += velocity * _timeStep;
	totalForce = vec3(0);
	totalTorque = vec3(0);
}
void Rigidbody::AddForce(vec3 _force) {
	AddForceAtPosition(_force, transform->position);
}
void Rigidbody::AddForceAtPosition(vec3 _force, vec3 _position) {
	vec3 torque = glm::cross(_position - transform->position, _force);
	totalForce += _force;
	AddTorque(torque);
}
void Rigidbody::AddTorque(vec3 _torque) {
	totalTorque += _torque;
}
void Rigidbody::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("Rigidbody")) {
		vec3 oldGUIVelocity = velocity;
		float oldGUIDynamicFriction = dynamicFriction;
		float oldGUIMass = mass;
		float oldGUIDrag = drag;
		float oldGUIDensity = density;
		float oldGUIAngularDrag = angularDrag;
		bool oldGUIUseGravity = useGravity;
		bool oldGUIIsKinematic = isKinematic;
		bool oldGUIEnabled = enabled;

		// Settings go here
		ImGui::DragFloat3("Velocity", (float*)&velocity);
		ImGui::DragFloat("DynamicFriction", &dynamicFriction);
		ImGui::DragFloat("Mass", &mass, 0.01f, 0.01f, 10.0f);
		ImGui::DragFloat("Drag", &drag);
		ImGui::DragFloat("Density", &density);
		ImGui::DragFloat("Angular Drag", &angularDrag);
		ImGui::Checkbox("Use Gravity", &useGravity);
		ImGui::Checkbox("Is Kinematic", &isKinematic);
		ImGui::Checkbox("Enabled", &enabled);
		
		ImGui::TreePop();

		if (oldGUIVelocity != velocity ||
			oldGUIDynamicFriction != dynamicFriction ||
			oldGUIMass != mass ||
			oldGUIDrag != drag ||
			oldGUIDensity != density ||
			oldGUIAngularDrag != angularDrag ||
			oldGUIUseGravity != useGravity ||
			oldGUIIsKinematic != isKinematic ||
			oldGUIEnabled != enabled) {
			isChangedInGUI = true;
		} else {
			isChangedInGUI = false;
		}
	}
	ImGui::EndChild();
	ImGui::End();
}
void Rigidbody::CalculateMomentOfInertia() {
	momentOfInertia = 0;
	if (gameObject != nullptr) {
		vector<Collider*>& colliders = gameObject->colliders;
		for (unsigned int i = 0; i < colliders.size(); ++i) {
			switch (colliders[i]->shapeId) {
			case SHAPE_PLANE:
				// Note(Manny): Implement plane's moment of inertia when possible.
				break;
			case SHAPE_SPHERE: {
				SphereCollider* sphere = dynamic_cast<SphereCollider*>(colliders[i]);
				momentOfInertia += (mass * (sphere->radius * sphere->radius)) / 2.0f;
				break;
			} 
			case SHAPE_BOX: {
				BoxCollider* box = dynamic_cast<BoxCollider*>(colliders[i]);
				float w = 2.0f * box->bounds.size.x;
				float h = 2.0f * box->bounds.size.y;
				float d = 2.0f * box->bounds.size.z;
				float Ih = 1.0f / 12.0f * mass * (w * w + d * d);
				float Iw = 1.0f / 12.0f * mass * (h * h + d * d);
				float Id = 1.0f / 12.0f * mass * (h * h + w * w);
				momentOfInertia += Ih + Iw + Id;
				break;
			} 
			default:
				break;
			}
		}
		momentOfInertia = (colliders.size() > 0 ? momentOfInertia / colliders.size() : 0);
	} else {
		std::cout << "ERROR IN RIGIDBODY!" << std::endl;
	}
}
bool Rigidbody::HasChangedInGUI() {
	return isChangedInGUI;
}