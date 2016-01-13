#include "Input.h"

// Utilities
#include "Window.h"

Input* Input::instance = nullptr;

Input::Input(){}
Input::~Input(){}
void Input::Create() {
	if (instance == nullptr) {
		instance = new Input();
	}
}
void Input::Shutdown() {
	if (instance) {
		delete instance;
	}
}
void Input::Update() {
	map<int, bool>& currButtons = instance->m_currButtons;
	map<int, bool>& prevButtons = instance->m_prevButtons;
	prevButtons = currButtons;

	for (auto button : currButtons) {
		if (glfwGetMouseButton(Window::window, button.first) == GLFW_PRESS) {
			button.second = true;
		} else {
			button.second = false;
		}
	}

	map<int, bool>& currKeys = instance->m_currKeys;
	map<int, bool>& prevKeys = instance->m_prevKeys;
	prevKeys = currKeys;

	for (auto key : currKeys) {
		if (glfwGetMouseButton(Window::window, key.first) == GLFW_PRESS) {
			key.second = true;
		} else {
			key.second = false;
		}
	}
}
vec3 Input::GetMousePosition() {
	double x, y;
	glfwGetCursorPos(Window::window, &x, &y);
	return vec3((float)x, (float)y, 0);
}
bool Input::GetMouseButton(int _button) {
	if (glfwGetMouseButton(Window::window, _button) == GLFW_PRESS) {
		return true;
	}
	return false;
}
bool Input::GetMouseButtonDown(int _button) {
	map<int, bool>& currButtons = instance->m_currButtons;
	map<int, bool>& prevButtons = instance->m_prevButtons;
	CheckMouseButtonExists(currButtons, _button);
	if (currButtons[_button] && !prevButtons[_button]) {
		return true;
	}	
	return false;
}
bool Input::GetMouseButtonUp(int _button) {
	map<int, bool>& currButtons = instance->m_currButtons;
	map<int, bool>& prevButtons = instance->m_prevButtons;
	CheckMouseButtonExists(currButtons, _button);
	if (!currButtons[_button] && prevButtons[_button]) {
		return true;
	}
	return false;
}
bool Input::GetKey(int _key) {
	if (glfwGetKey(Window::window, _key) == GLFW_PRESS) {
		return true;
	}
	return false;
}
bool Input::GetKeyDown(int _key) {
	map<int, bool>& currKeys = instance->m_currKeys;
	map<int, bool>& prevKeys = instance->m_prevKeys;
	CheckKeyExists(currKeys, _key);
	if (currKeys[_key] && !prevKeys[_key]) {
		return true;
	}
	return false;
}
bool Input::GetKeyUp(int _key) {
	map<int, bool>& currKeys = instance->m_currKeys;
	map<int, bool>& prevKeys = instance->m_prevKeys;
	CheckKeyExists(currKeys, _key);
	if (!currKeys[_key] && prevKeys[_key]) {
		return true;
	}
	return false;
}
bool Input::CheckMouseButtonExists(map<int, bool>& _buttons, int _button) {
	if (_buttons.find(_button) != _buttons.end()) {
		map<int, bool>& currButtons = instance->m_currButtons;
		map<int, bool>& prevButtons = instance->m_prevButtons;
		if (glfwGetMouseButton(Window::window, _button) == GLFW_PRESS) {
			currButtons[_button] = true;
		} else {
			currButtons[_button] = false;
		}
		return true;
	}
	return false;
}
bool Input::CheckKeyExists(map<int, bool>& _keys, int _key) {
	if (_keys.find(_key) != _keys.end()) {
		map<int, bool>& currKeys = instance->m_currKeys;
		map<int, bool>& prevKeys = instance->m_prevKeys;
		if (glfwGetKey(Window::window, _key) == GLFW_PRESS) {
			currKeys[_key] = true;
		} else {
			currKeys[_key] = false;
		}
		return true;
	}
	return false;
}