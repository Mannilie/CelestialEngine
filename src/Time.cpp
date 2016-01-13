#include "Time.h"

Time* Time::instance;
float Time::deltaTime;
float Time::elapsedTime;
float Time::fps;

Time::Time(){}

void Time::Create() {
	if (instance == nullptr) {
		instance = new Time();
	}
	instance->deltaTime = 0;
	instance->elapsedTime = 0;
	instance->fps = 0;
}
bool Time::Update() {
	instance->deltaTime = (float)glfwGetTime();
	glfwSetTime(0.0f);
	instance->elapsedTime += instance->deltaTime;
	instance->fps = 1.0f / instance->deltaTime;
	return true;
}