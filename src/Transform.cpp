#include "Transform.h"

// Objects
#include "GameObject.h"

// Components
#include "Camera.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "Rigidbody.h"
#include "ParticleEmitter.h"
#include "Lighting.h"
#include "MeshRenderer.h"

// Structs
#include "Bounds.h"
#include "Ray.h"

// GUI
#include "imgui.h"

// Debugging
#include "Gizmos.h"

// Utilities
#include "Input.h"

int Transform::transformCount = 0;
map<int, Transform*> Transform::sm_transforms;

// Public
Transform::Transform(vec3 _position, quat _rotation, vec3 _scale) :
	position(_position),
	scale(_scale),
	rotation(_rotation),
	eulerAngles(glm::eulerAngles(_rotation)),
	forward(vec3(1, 0, 0)),
	up(vec3(0, 1, 0)),
	right(vec3(0, 0, 1)),
	isSelectable(true),
	isSelected(false),
	initializedOldStuff(false),
	isChangedInGUI(false),
	transformID(transformCount++),
	parent(nullptr) {
	sm_transforms[transformID] = this;
}
Transform::~Transform() {
	map<int, Transform*>::iterator it = sm_transforms.find(transformID);
	if (it != sm_transforms.end()) {
		sm_transforms.erase(it);
	}
}
bool Transform::Update() {
	if (isSelected) { Inspector(); }
	
	if (initializedOldStuff) {
		oldPosition = position;
		oldEulerAngles = eulerAngles;
		oldRotation = rotation;
		oldScale = scale;
		oldModelMatrix = worldMatrix;
	} else {
		oldPosition = position + vec3(1);
		oldRotation = rotation + quat();
		oldEulerAngles = glm::eulerAngles(rotation);
		oldScale = scale + vec3(1);
		oldModelMatrix = worldMatrix + mat4(1);
		initializedOldStuff = true;
	}
	
	UpdateWorldMatrix();

	return true;
}
void Transform::UpdateWorldMatrix() {
	matTranslation = glm::translate(mat4(1.0f), position);
	matScale = glm::scale(scale);
	matRotation	= glm::toMat4(rotation);

	worldMatrix = matTranslation * matRotation * matScale;

	right = TransformDirection(vec3(1, 0, 0));
	up = TransformDirection(vec3(0, 1, 0));
	forward = TransformDirection(vec3(0, 0, 1));
}
void Transform::Inspector() {
	ImGui::Begin("Inspector", 0, 0 | ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Add Component")) {
			// Components go here

			if (ImGui::MenuItem("MeshRenderer")) {
				gameObject->AddComponent<MeshRenderer>(MeshRenderer(Mesh("cube.obj")))->Startup();
			}

			// Lighting
			if (ImGui::BeginMenu("Lighting")) {
				if (ImGui::MenuItem("DirectionalLight")) {
					gameObject->AddComponent<DirectionalLight>()->Startup();
				}
				if (ImGui::MenuItem("PointLight")) {
					gameObject->AddComponent<PointLight>()->Startup();
				}
				ImGui::EndMenu();
			}

			// Physics
			if (ImGui::BeginMenu("Physics")) {
				if (ImGui::MenuItem("Rigidbody")) {
					gameObject->AddComponent<Rigidbody>()->Startup();
				}
				if (ImGui::MenuItem("ParticleEmitter")) {
					gameObject->AddComponent<ParticleEmitter>()->Startup();
				}
				ImGui::EndMenu();
			}

			// Colliders
			if (ImGui::BeginMenu("Colliders")) {
				if (ImGui::MenuItem("BoxCollider")) {
					gameObject->AddComponent<BoxCollider>()->Startup();
				}
				if (ImGui::MenuItem("SphereCollider")){
					gameObject->AddComponent<SphereCollider>()->Startup();
				}
				if (ImGui::MenuItem("PlaneCollider")) {
					gameObject->AddComponent<PlaneCollider>()->Startup();
				}
				if (ImGui::MenuItem("CapsuleCollider")) {
					gameObject->AddComponent<CapsuleCollider>()->Startup();
				}
				if (ImGui::MenuItem("MeshCollider")) {
					gameObject->AddComponent<MeshCollider>()->Startup();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

	ImGui::Begin("Inspector");
	ImGui::SetNextTreeNodeOpened(true, ImGuiSetCond_Once);
	if (ImGui::TreeNode("Transform")) {
		vec3 oldGUIPosition = position;
		vec3 oldGUIEulerAngles = eulerAngles;
		vec3 oldGUIScale = scale;

		ImGui::DragFloat3("Position", (float*)&position, 0.01f);
		ImGui::DragFloat3("Rotation", (float*)&eulerAngles, 0.1f);
		ImGui::DragFloat3("Scale", (float*)&scale, 0.01f);
		ImGui::TreePop();

		if (oldGUIPosition != position ||
			oldGUIEulerAngles != eulerAngles ||
			oldGUIScale != scale) {
			isChangedInGUI = true;
		} else {
			isChangedInGUI = false;
		}

		if (oldGUIEulerAngles != eulerAngles) {
			rotation = glm::toQuat(glm::yawPitchRoll(glm::radians(eulerAngles.y),
													 glm::radians(eulerAngles.x),
													 glm::radians(eulerAngles.z)));
		}
	}
	ImGui::Text("Components");
	ImGui::End();
}
bool Transform::HasChanged() {
	if (position != oldPosition) { return true; }
	if (rotation != oldRotation) { return true; }
	if (scale != scale) { return true; }
	return false;
}
bool Transform::HasChangedInGUI() {
	return isChangedInGUI;
}
void Transform::Rotate(const vec3& _eulerAngles) {
	eulerAngles = _eulerAngles;
	rotation = glm::toQuat(glm::yawPitchRoll(glm::radians(eulerAngles.y),
		glm::radians(eulerAngles.x),
		glm::radians(eulerAngles.z)));
}
void Transform::Rotate(const vec3& _axis, float _angle) {
	Rotate(quat(_angle, _axis));
}
void Transform::Rotate(const quat& _rotation) {
	rotation = quat(glm::normalize(_rotation * rotation));
	eulerAngles = glm::degrees(glm::eulerAngles(rotation));
	MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
	if (meshRenderer != nullptr) {
		meshRenderer->mesh.CalculateMeshBounds();
	}
}
void Transform::LookAt(const vec3& _toLookAt) {
	mat4 lookingMatrix = glm::inverse(glm::lookAt(position, _toLookAt, up));
	lookingMatrix = glm::extractMatrixRotation(lookingMatrix);
	rotation = glm::toQuat(lookingMatrix);
	eulerAngles = glm::degrees(glm::eulerAngles(rotation));
} 
void Transform::SetParent(Transform* _transform) {
	_transform->children.push_back(this);
	parent = _transform;
}
void Transform::SetParent(GameObject* _gameObject) {
	_gameObject->transform.children.push_back(this);
	parent = &_gameObject->transform;
}
vec3 Transform::TransformDirection(vec3& _direction) {
	return (matRotation * vec4(_direction, 0)).xyz();
}
vec3 Transform::TransformDirectionInverse(vec3& _direction) {
	return (glm::transpose(matRotation) * vec4(_direction, 0)).xyz();
}

// Static
void Transform::SetSelectedTransform(Transform* _transform) {
	DeselectAllTransforms();
	_transform->isSelected = true;
}
void Transform::UpdateTransformSelection() {
	vector<SelectedTransform> selectedTransforms;
	bool mousePressed = false;
	for (auto trans : sm_transforms) {
		Transform* transform = trans.second;
		if (transform->isSelectable) {
			if (transform->gameObject == nullptr) {
				continue;
			}
			MeshRenderer* meshRenderer = transform->gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr) {
				vec3 mousePos = Input::GetMousePosition();
				RaycastHit hitInfo;
				if (meshRenderer->bounds.RayCast(Camera::current->ScreenPointToRay(mousePos), &hitInfo, 10000.0f)) {
					if (!ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsMouseDragging(0)) {
						if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_1)) {
							mousePressed = true;
							SelectedTransform newSelectedTransform;
							newSelectedTransform.transform = transform;
							newSelectedTransform.hitInfo = hitInfo;
							selectedTransforms.push_back(newSelectedTransform);
						}
					}
				}
			}
		}
	}

	if (selectedTransforms.empty() && mousePressed == true) {
		DeselectAllTransforms();
	} else if (selectedTransforms.size() > 0) {
		Transform* closestTransform = nullptr;
		float min = FLT_MAX;
		for (unsigned int i = 0; i < selectedTransforms.size(); ++i) {
			float currentDistance = selectedTransforms[i].hitInfo.distance;
			if (currentDistance < min) {
				closestTransform = selectedTransforms[i].transform;
				min = currentDistance;
			}
		} if (closestTransform != nullptr) {
			SetSelectedTransform(closestTransform);
		}
	}
}
Transform* Transform::GetSelectedTransform() {
	for (auto transform : sm_transforms) {
		if (transform.second->isSelected) {
			return transform.second;
		}
	}
	return nullptr;
}
void Transform::DeselectAllTransforms() {
	for (auto transform : sm_transforms) {
		transform.second->isSelected = false;
	}
}