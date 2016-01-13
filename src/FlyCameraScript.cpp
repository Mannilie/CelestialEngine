#include "FlyCameraScript.h"

// Object
#include "GameObject.h"

// Components
#include "CharacterController.h"
#include "Transform.h"
#include "Camera.h"

// GUI
#include "imgui.h"

// Other
#include "Input.h"
#include "Time.h"

FlyCameraScript::FlyCameraScript() :
	mouseSensitivity(20.0f),
	movementSpeed(20.0f) {}
bool FlyCameraScript::Startup(){ return true; }
void FlyCameraScript::Shutdown(){}
bool FlyCameraScript::Update() {	
	if (this->transform->isSelected) { Inspector(); }

	if (Camera::current->gameObject == this->gameObject) {
		if (Input::GetKey(GLFW_KEY_W)) {
			transform->position += -transform->forward * movementSpeed * Time::deltaTime;
		}

		if (Input::GetKey(GLFW_KEY_S)) {
			transform->position += transform->forward * movementSpeed * Time::deltaTime;
		}

		if (Input::GetKey(GLFW_KEY_A)) {
			transform->position += -transform->right * movementSpeed * Time::deltaTime;
		}

		if (Input::GetKey(GLFW_KEY_D)) {
			transform->position += transform->right * movementSpeed * Time::deltaTime;
		}

		if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
			glfwGetCursorPos(Window::window, &oldMouseX, &oldMouseY);
			glfwSetCursorPos(Window::window, Window::width * 0.5f, Window::height* 0.5f);
			glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		} else if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)) {
			double mouseX, mouseY;
			glfwGetCursorPos(Window::window, &mouseX, &mouseY);
			glfwSetCursorPos(Window::window, Window::width * 0.5f, Window::height * 0.5f);

			mouseX -= Window::width * 0.5f;
			mouseY -= Window::height * 0.5f;

			mouseX /= Window::width * 0.5f;
			mouseY /= Window::height * 0.5f;

			mouseX *= -mouseSensitivity;
			mouseY *= -mouseSensitivity;

			float pitch = (float)mouseY + transform->eulerAngles.x;
			float yaw = (float)mouseX + transform->eulerAngles.y;

			if (pitch >= 90.0f) {
				pitch = 90.0f;
			}

			if (pitch <= -90.0f) {
				pitch = -90.0f;
			}

			transform->Rotate(vec3(pitch, yaw, 0));
		}

		if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_RIGHT)) {
			glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(Window::window, oldMouseX, oldMouseY);
		}
	}

	return true;
}
void FlyCameraScript::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("FlyCameraScript")) {
		// Settings go here
		ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f);
		ImGui::DragFloat("Movement Speed", &movementSpeed);
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}