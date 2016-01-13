#include "Debug.h"

// GUI
#include "imgui.h"

// Other
#include <iostream>

Debug* Debug::instance = nullptr;

Debug::Debug(){}
Debug::~Debug(){}
void Debug::Create() {
	if (instance == nullptr) {
		instance = new Debug();
	}
}
bool Debug::Update() {
	ImGui::Begin("Console");
	for (unsigned int i = 0; i < instance->text.size(); ++i) {
		ImGui::TextColored(ImVec4(1, 1, 1, 1), (std::to_string(instance->text[i].callCount) + instance->text[i].text).c_str());
	}
	for (unsigned int i = 0; i < instance->errors.size(); ++i) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), (std::to_string(instance->errors[i].callCount) + "[error] " + instance->errors[i].text).c_str());
	}
	for (unsigned int i = 0; i < instance->warnings.size(); ++i) {
		ImGui::TextColored(ImVec4(1, 1, 0, 1), (std::to_string(instance->warnings[i].callCount) + "[warning] " + instance->warnings[i].text).c_str());
	}
	ImGui::End();
	return true;
}
void Debug::Log(string _text) {
	if (TextExists(_text, instance->text)) { return; }
	std::cout << _text << std::endl;
	instance->text.push_back(_text);
}
void Debug::Log(string _text, Object* _object) {
	if (TextExists(_text, instance->text)) { return; }
	std::cout << _text << std::endl;
	instance->text.push_back(_text);
}
void Debug::LogError(string _error) {
	if (TextExists(_error, instance->errors)) { return; }
	std::cout << string("[error] ") + _error << std::endl;
	instance->errors.push_back(_error);
}
void Debug::LogError(string _error, Object* _object) {
	if (TextExists(_error, instance->errors)) { return; }
	std::cout << string("[error] ") + _error << std::endl;
	instance->errors.push_back(TextLog(_error));
}
void Debug::LogWarning(string _warning) {
	if (TextExists(_warning, instance->warnings)) { return; }
	std::cout << string("[warning] ") + _warning << std::endl;
	instance->warnings.push_back(_warning);
}
void Debug::LogWarning(string _warning, Object* _object) {
	if (TextExists(_warning, instance->warnings)) { return; }
	std::cout << string("[warning] ") + _warning << std::endl;
	instance->warnings.push_back(_warning);
}
bool Debug::HasError() {
	return instance->errors.size() > 0;
}
bool Debug::TextExists(string _text, vector<TextLog>& _textLog) {
	for (unsigned int i = 0; i < _textLog.size(); ++i) {
		if (_textLog[i].text == _text) {
			_textLog[i].callCount++;
			return true;
		}
	}
	return false;
}