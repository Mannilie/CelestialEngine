// Components
#include "CharacterController.h"
#include "Transform.h"
#include "GameObject.h"

// GUI
#include "imgui.h"

// Debugging
#include "Debug.h"
#include "Gizmos.h"

// Other
#include "Input.h"
#include "Time.h"

FirstPersonCameraScript::FirstPersonCameraScript() :
	maxY(75.0f),
	minY(-75.0f),
	mouseSensitivity(50.0f),
	jumpHeight(20.0f) {}

bool FirstPersonCameraScript::Startup(){ return true; }
void FirstPersonCameraScript::Shutdown(){}
bool FirstPersonCameraScript::Update()
{
	if (this->transform->isSelected) Inspector();

	if (Camera::current->gameObject == this->gameObject)
	{
		if (transform->parent)
		{
			CharacterController* characterController = transform->parent->gameObject->GetComponent<CharacterController>();
			if (characterController)
			{
				vec3& velocity = characterController->velocity;
				float& movementSpeed = characterController->movementSpeed;
				bool isGrounded = characterController->isGrounded;
				float& yRotation = characterController->yRotation;

				vec3 forward = transform->parent->forward;
				vec3 right = transform->parent->right;
				vec3 up = transform->parent->up;

				velocity.x = 0;
				velocity.z = 0;
				
				vec3 vel = vec3(0);

				Gizmos::addLine(transform->parent->position, transform->parent->position + forward * 50.0f, vec4(0, 0, 1, 1));
				Gizmos::addLine(transform->parent->position, transform->parent->position + right * 50.0f, vec4(0, 0, 1, 1));
				Gizmos::addLine(transform->parent->position, transform->parent->position + up * 50.0f, vec4(0, 0, 1, 1));

				if (Input::GetKey(GLFW_KEY_W))
					vel += -forward * movementSpeed * Time::deltaTime;
				if (Input::GetKey(GLFW_KEY_S))
					vel += forward * movementSpeed * Time::deltaTime;
				if (Input::GetKey(GLFW_KEY_A))
					vel += -right * movementSpeed * Time::deltaTime;
				if (Input::GetKey(GLFW_KEY_D))
					vel += right * movementSpeed * Time::deltaTime;

				velocity = vec3(vel.x, velocity.y, vel.z);

				if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
				{
					glfwGetCursorPos(Window::window, &oldMouseX, &oldMouseY);
					glfwSetCursorPos(Window::window, Window::width * 0.5f, Window::height* 0.5f);
					glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				}
				else if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
				{
					double mouseX, mouseY;
					glfwGetCursorPos(Window::window, &mouseX, &mouseY);
					glfwSetCursorPos(Window::window, Window::width * 0.5f, Window::height * 0.5f);

					mouseX -= Window::width * 0.5f;
					mouseY -= Window::height * 0.5f;

					mouseX /= Window::width * 0.5f;
					mouseY /= Window::height * 0.5f;

					mouseX *= -mouseSensitivity;
					mouseY *= -mouseSensitivity;

					float pitch = transform->eulerAngles.x + (float)mouseY;
					
					if (pitch >= maxY)
						pitch = maxY;

					if (pitch <= minY)
						pitch = minY;

					transform->Rotate(vec3(pitch, 0, 0));
					
					yRotation += glm::radians((float)mouseX);

				}
				if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_RIGHT))
				{
					glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					glfwSetCursorPos(Window::window, oldMouseX, oldMouseY);
				}

				if (isGrounded)
				{
					if (Input::GetKeyDown(GLFW_KEY_SPACE))
					{
						velocity += vec3(0, 1, 0) * jumpHeight * Time::deltaTime;
					}
				}

			}
		}
	}

	return true;
}

void FirstPersonCameraScript::Draw(RenderingEngine& _renderer)
{

}

void FirstPersonCameraScript::Inspector()
{
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("FirstPersonCameraScript"))
	{
		ImGui::DragFloat("Mouse Sensitivity", &mouseSensitivity);
		ImGui::DragFloat("Jump Height", &jumpHeight);
		
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}