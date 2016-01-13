/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Camera.h
@date: 13/06/2015
@author: Emmanuel Vaccaro
@brief: A way for the player to view the world 
===============================================*/

#ifndef _CAMERA_H_
#define _CAMERA_H_

// Maths
#include "GLM_Header.h"

// Components
#include "Component.h"

// Structs
#include "Ray.h"
#include "Color.h"

// Utilities
#include "Window.h"

// Other
#include <string>
using std::string;
using std::to_string;

// Forward declaration
struct GLFWwindow;
class GameObject;

class Camera : public Component {
public:
	Camera(const mat4& _projectionMatrix, Transform* _transform = nullptr);
	Camera(float _aspect = Window::width / Window::height,
		float _fieldOfView = 60.0f,
		float _farClipPlane = 1000.0f,
		float _nearClipPlane = 0.1f,
		bool _orthographic = false,
		vec3 _velocity = vec3(20),
		float _mouseSensitivity = 20.0f);
	virtual ~Camera();
	bool Startup();
	void Shutdown();
	bool Update();
	void Draw(RenderingEngine& _renderer);
	Ray ScreenPointToRay(vec3 _position);
	vec3 ScreenToWorldPoint(vec3 _position);
	void UpdateProjection();
	void RenderFrustum();
	bool HasChanged();
	void Inspector();
	mat4 ProjectionLerp(const mat4& _sourceProjection, 
		const mat4& _destProjection, 
		float _smoothness);

	static int allCamerasCount;
	static Camera* current;
	static Camera* main;
	mat4 perspectiveMatrix;
	mat4 orthographicMatrix;
	mat4 projectionMatrix;
	mat4 viewMatrix;
	Color backgroundColor;
	vec3 velocity;
	float aspect;
	float fieldOfView;
	float farClipPlane;
	float nearClipPlane;
	float orthographicSize;
	float mouseSensitivity;
	bool orthographic;
	mutable mat4 oldProjectionMatrix;
	mutable mat4 oldViewMatrix;
	mutable mat4 oldRotationMatrix;
	mutable double oldMouseX;
	mutable double oldMouseY;
	mutable bool oldOrthographic;
	mutable bool initializedOldStuff;
private:
	float perspNear;
	float perspFar;
	float orthoNear;
	float orthoFar;
	float perspSmoothness;
	float orthoSmoothness;
	float startLerpTime;
	bool projectionChanging;
};

#endif // _CAMERA_H_