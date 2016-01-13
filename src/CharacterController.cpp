#include "CharacterController.h"

#include "PhysicsEngine.h"
#include "CoreEngine.h"

#include "Input.h"
#include "Time.h"

#include "Gizmos.h"
#include "Transform.h"

#include "imgui.h"

CharacterController::CharacterController() : yRotation(0.0f),
	velocity(vec3(0)),
	height(20.0f),
	radius(5.0f),
	gravity(-0.907f),
	density(10.0f),
	slopeLimit(0.707f),
	stepOffset(0.5f),
	movementSpeed(20.0f),
	useGravity(true),
	isGrounded(false),
	enabled(true),
	isChangedInGUI(false) {}

CharacterController::~CharacterController(){}

bool CharacterController::Startup() {
	CoreEngine::physics->AddActor(this);
	return true;
}

void CharacterController::Shutdown() {
	CoreEngine::physics->RemoveActor(this);
}

bool CharacterController::Update() {
	if (this->transform->isSelected) { Inspector(); }
	return true;
}

void CharacterController::Draw(RenderingEngine& _renderer) {
	Gizmos::AddCapsule(this->transform->position, height, radius, 16, 16, vec4(1, 0, 0, 1), vec3(0, 1, 0), &transform->matRotation);
}

void CharacterController::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), false);
	if (ImGui::TreeNode("CharacterController")) {
		vec3 oldGUIVelocity = velocity;
		float oldGUIHeight = height;
		float oldGUIRadius = radius;
		float oldGUIGravity = gravity;
		float oldGUIDensity = density;
		float oldGUISlopeLimit = slopeLimit;
		float oldGUIStepOffset = stepOffset;
		float oldGUIMovementSpeed = movementSpeed;
		bool oldGUIUseGravity = useGravity;
		bool oldGUIIsGrounded = isGrounded;
		bool oldGUIEnabled = enabled;

		// Settings go here
		ImGui::DragFloat3("Velocity", (float*)&velocity);
		ImGui::DragFloat("Height", &height);
		ImGui::DragFloat("Radius", &radius);
		ImGui::DragFloat("Gravity", &gravity);
		ImGui::DragFloat("Density", &density);
		ImGui::DragFloat("Slope Limit", &slopeLimit);
		ImGui::DragFloat("Step Offset", &stepOffset);
		ImGui::DragFloat("Movement Speed", &movementSpeed);
		ImGui::Checkbox("Use Gravity", &useGravity);
		ImGui::Checkbox("Is Grounded", &isGrounded);
		ImGui::Checkbox("Enabled", &enabled);

		ImGui::TreePop();

		if (oldGUIVelocity != velocity ||
			oldGUIHeight != height ||
			oldGUIRadius != radius ||
			oldGUIGravity != gravity ||
			oldGUIDensity != density ||
			oldGUISlopeLimit != slopeLimit ||
			oldGUIStepOffset != stepOffset ||
			oldGUIMovementSpeed != movementSpeed ||
			oldGUIUseGravity != useGravity ||
			oldGUIIsGrounded != isGrounded ||
			oldGUIEnabled != enabled) {
			isChangedInGUI = true;
		} else {
			isChangedInGUI = false;
		}
	}
	ImGui::EndChild();
	ImGui::End();
}