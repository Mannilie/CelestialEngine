#include "Camera.h"
#include "GLFW_Header.h"
#include "Window.h"
#include "GameObject.h"
#include "Gizmos.h"
#include "Input.h"
#include "imgui.h"
#include "Time.h"
#include <math.h>

// Camera
int	Camera::allCamerasCount = 0;
Camera* Camera::current = nullptr;
Camera* Camera::main = nullptr;

Camera::Camera(const mat4& _projectionMatrix, 
	Transform* _transform) :
	aspect((float)Window::width / (float)Window::height),
	projectionMatrix(_projectionMatrix),
	fieldOfView(60.0f),
	farClipPlane(1000.0f),
	nearClipPlane(0.1f),
	orthographic(false),
	orthographicSize(10.0f),
	velocity(vec3(20.0f)),
	mouseSensitivity(20.0f),
	backgroundColor(Color(0.1f, 0.48f, 0.86f, 1.0f)),
	perspSmoothness(10.0f),
	orthoSmoothness(20.0f),
	initializedOldStuff(false) {
	transform = _transform;
	orthoNear = perspNear = nearClipPlane;
	orthoFar = perspFar = farClipPlane;
	allCamerasCount++;
}
Camera::Camera(float _aspect,
	float _fieldOfView,
	float _farClipPlane,
	float _nearClipPlane,
	bool _orthographic,
	vec3 _velocity,
	float _mouseSensitivity) : aspect(_aspect),
	fieldOfView(_fieldOfView),
	farClipPlane(_farClipPlane),
	nearClipPlane(_nearClipPlane),
	orthographic(_orthographic),
	velocity(_velocity),
	mouseSensitivity(_mouseSensitivity),
	backgroundColor(Color(0.1f, 0.48f, 0.86f, 1)),
	orthographicSize(10.0f),
	perspSmoothness(10.0f),
	orthoSmoothness(20.0f),
	startLerpTime(0.0f),
	initializedOldStuff(false) {
	orthoNear = perspNear = nearClipPlane;
	orthoFar = perspFar = farClipPlane;
	allCamerasCount++;
}
Camera::~Camera(){}
bool Camera::Startup() { return true; }
void Camera::Shutdown() {}
bool Camera::Update() {
	aspect = (float)Window::width / (float)Window::height;

	if (Camera::current == this) {
		ImGui::Begin("Main Camera Settings");
		ImGui::Checkbox("Orthographic", &orthographic);

		if (orthographic) {
			ImGui::DragFloat("Orthographic Size", &orthographicSize);
			ImGui::DragFloat("Near Clip Plane", (float*)&orthoNear);
			ImGui::DragFloat("Far Clip Plane", (float*)&orthoFar);
		} else {
			ImGui::DragFloat("Field Of View", &fieldOfView);
			ImGui::DragFloat("Near Clip Plane", (float*)&perspNear);
			ImGui::DragFloat("Far Clip Plane", (float*)&perspFar);
		}

		ImGui::ColorEdit4("Background Color", (float*)&backgroundColor);
		ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f);
		ImGui::DragFloat3("Velocity", (float*)&velocity);

		ImGui::End();
	}

	if (this->transform && this->transform->isSelected) { Inspector(); }

	if (initializedOldStuff) {
		oldProjectionMatrix = projectionMatrix;
		oldViewMatrix = viewMatrix;
	} else {
		oldProjectionMatrix = projectionMatrix + mat4(1);
		oldViewMatrix = viewMatrix + mat4(1);
		oldOrthographic = orthographic;
		initializedOldStuff = true;
	}

	UpdateProjection();
	
	if(Camera::current != this) RenderFrustum();

	return true;
}
void Camera::Draw(RenderingEngine& _renderer) {
	if (Camera::current == this) {
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	}
}
Ray Camera::ScreenPointToRay(vec3 _position) {
	float nxPos = _position.x / Window::width;
	float nyPos = _position.y / Window::height;

	float sxPos = nxPos - 0.5f;
	float syPos = nyPos - 0.5f;

	float fxPos = sxPos;
	float fyPos = syPos;

	if (!orthographic) {
		fxPos *= 2;
		fyPos *= -2;
	} else {
		fxPos *= orthographicSize / 2;
		fyPos *= -orthographicSize / 2;
	}

	mat4 invViewProj = glm::inverse(projectionMatrix * viewMatrix);
	vec4 mousePos(fxPos, fyPos, -1, 1);
	vec4 worldScreenPoint = invViewProj * mousePos;
	worldScreenPoint /= worldScreenPoint.w;

	vec3 rayOrigin;
	vec3 dir;

	rayOrigin = transform->worldMatrix[3].xyz();
	if (!orthographic) {
		rayOrigin = transform->worldMatrix[3].xyz();
	}

	dir = rayOrigin - worldScreenPoint.xyz;

	return Ray(rayOrigin, dir);
}
vec3 Camera::ScreenToWorldPoint(vec3 _position) {
	float nxPos = _position.x / Window::width;
	float nyPos = _position.y / Window::height;

	float sxPos = nxPos - 0.5f;
	float syPos = nyPos - 0.5f;

	float fxPos = sxPos * 2;
	float fyPos = syPos * -2;

	mat4 invViewProj = glm::inverse(projectionMatrix * viewMatrix);
	vec4 mousePos(fxPos, fyPos, 1, 1);
	vec4 worldScreenPoint = invViewProj * mousePos;

	worldScreenPoint /= worldScreenPoint.w;

	vec3 camPos = transform->worldMatrix[3].xyz();

	return camPos;
}
void Camera::UpdateProjection() {
	viewMatrix = glm::inverse(transform->worldMatrix);

	if (aspect != aspect) { aspect = 0; }

	if (fieldOfView <= 0) { fieldOfView = 0.1f; }
	if (nearClipPlane <= 0)  { nearClipPlane = 0.1f; }
	if (farClipPlane <= nearClipPlane) { farClipPlane = nearClipPlane + 0.1f; }
	if (perspNear < 0.1f) { perspNear = 0.1f; }

	if (!orthographic) {
		nearClipPlane = perspNear;
		farClipPlane = perspFar;
		perspectiveMatrix = glm::perspective(glm::radians(fieldOfView), aspect, nearClipPlane, farClipPlane);
	} else {
		nearClipPlane = orthoNear;
		farClipPlane = orthoFar;
		orthographicMatrix = glm::ortho<float>(-(orthographicSize * 0.5f) * aspect,
			(orthographicSize * 0.5f) * aspect,
			-(orthographicSize * 0.5f),
			orthographicSize * 0.5f,
			nearClipPlane,
			farClipPlane);
	}

	if (oldOrthographic != orthographic) { projectionChanging = true; }

	if (!projectionChanging) {
		projectionMatrix = orthographic ? orthographicMatrix : perspectiveMatrix;
	} else {
		startLerpTime += Time::deltaTime;

		projectionMatrix = ProjectionLerp(projectionMatrix, orthographic ? orthographicMatrix : perspectiveMatrix,
			orthographic ? orthoSmoothness * Time::deltaTime : perspSmoothness * Time::deltaTime);

		if (startLerpTime >= 1.0f) {
			projectionChanging = false;
			startLerpTime = 0.0f;
		}
	}
}
void Camera::RenderFrustum() {
	float nearHeight, nearWidth;
	float farHeight, farWidth;

	if (!orthographic) {
		nearHeight = 2 * tan(glm::radians(fieldOfView / 2)) * nearClipPlane;
		nearWidth = nearHeight * aspect;
		farHeight = 2 * tan(glm::radians(fieldOfView / 2)) * farClipPlane;
		farWidth = farHeight * aspect;
	} else {
		nearHeight = farHeight = 2.f * orthographicSize;
		nearWidth = farWidth = nearHeight * aspect;
	}

	vec3 nearCenter		= transform->position + (transform->forward * nearClipPlane);
	vec3 farCenter		= transform->position + (transform->forward * farClipPlane);

	vec3 nearTopLeft		= nearCenter - transform->right * (nearWidth * 0.5f) + transform->up * (nearHeight * 0.5f);
	vec3 nearTopRight		= nearCenter + transform->right * (nearWidth * 0.5f) + transform->up * (nearHeight * 0.5f);
	vec3 nearBottomRight	= nearCenter + transform->right * (nearWidth * 0.5f) - transform->up * (nearHeight * 0.5f);
	vec3 nearBottomLeft		= nearCenter - transform->right * (nearWidth * 0.5f) - transform->up * (nearHeight * 0.5f);

	vec3 farTopLeft			= farCenter - transform->right * (farWidth * 0.5f) + transform->up * (farHeight * 0.5f);
	vec3 farTopRight		= farCenter + transform->right * (farWidth * 0.5f) + transform->up * (farHeight * 0.5f);
	vec3 farBottomLeft		= farCenter - transform->right * (farWidth * 0.5f) - transform->up * (farHeight * 0.5f);
	vec3 farBottomRight		= farCenter + transform->right * (farWidth * 0.5f) - transform->up * (farHeight * 0.5f);

	//Near Plane
	Gizmos::AddLine(nearTopLeft, nearTopRight, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearTopRight, nearBottomRight, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearBottomRight, nearBottomLeft, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearBottomLeft, nearTopLeft, vec4(0, 0, 0, 1));

	//In Between
	Gizmos::AddLine(nearTopLeft, farTopLeft, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearTopRight, farTopRight, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearBottomLeft, farBottomLeft, vec4(0, 0, 0, 1));
	Gizmos::AddLine(nearBottomRight, farBottomRight, vec4(0, 0, 0, 1));

	//Far Plane
	Gizmos::AddLine(farTopLeft, farTopRight, vec4(0, 0, 0, 1));
	Gizmos::AddLine(farTopRight, farBottomRight, vec4(0, 0, 0, 1));
	Gizmos::AddLine(farBottomRight, farBottomLeft, vec4(0, 0, 0, 1));
	Gizmos::AddLine(farBottomLeft, farTopLeft, vec4(0, 0, 0, 1));
}
bool Camera::HasChanged() {
	if (projectionMatrix != oldProjectionMatrix) { return true; }
	if (viewMatrix != oldViewMatrix) { return true; }
	return false;
}
void Camera::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("Camera")) {
		ImGui::Checkbox("Orthographic", &orthographic);

		if (orthographic) {
			ImGui::DragFloat("Orthographic Size", &orthographicSize);
			ImGui::DragFloat("Near Clip Plane", (float*)&orthoNear);
			ImGui::DragFloat("Far Clip Plane", (float*)&orthoFar);
		} else {
			ImGui::DragFloat("Field Of View", &fieldOfView);
			ImGui::DragFloat("Near Clip Plane", (float*)&perspNear);
			ImGui::DragFloat("Far Clip Plane", (float*)&perspFar);
		}

		ImGui::ColorEdit4("Background Color", (float*)&backgroundColor);
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}
mat4 Camera::ProjectionLerp(const mat4& _sourceProjection, 
	const mat4& _destProjection, 
	float _smoothness) {
	mat4 res;
	for (int i = 0; i < 4; ++i) {
		res[i] = glm::lerp(_sourceProjection[i], _destProjection[i], _smoothness);
	}
	return res;
}