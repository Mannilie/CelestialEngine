/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Transform.h
@date: 20/06/2015
@author: Emmanuel Vaccaro
@brief: Position, rotation and scale of an 
object.
===============================================*/

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

// Common
#include "GLM_Header.h"

// Structs
#include "Ray.h"

// Other
#include <map>
using std::map;
#include <vector>
using std::vector;

// Forward declaration
class GameObject;

class Transform {
public:
	Transform(vec3 _position = vec3(0), 
			  quat _rotation = quat(), 
			  vec3 _scale = vec3(1));
	virtual ~Transform();
	virtual bool Update();
	void UpdateWorldMatrix();
	void Inspector();
	bool HasChanged();
	bool HasChangedInGUI();
	void Rotate(const vec3& _eulerAngles);
	void Rotate(const vec3& _axis, float _angle);
	void Rotate(const quat& _rotation);
	void LookAt(const vec3& _toLookAt);
	void SetParent(Transform* _transform);
	void SetParent(GameObject* _gameObject);
	vec3 TransformDirection(vec3& _direction);
	vec3 TransformDirectionInverse(vec3& _direction);
	static void SetSelectedTransform(Transform* _transform);
	static void UpdateTransformSelection();
	static Transform* GetSelectedTransform();
	static void DeselectAllTransforms();

	static int transformCount;

	mat4 worldMatrix;
	mat4 matTranslation;
	mat4 matRotation;
	mat4 matScale;
	quat rotation;
	vec3 position;
	vec3 eulerAngles;
	vec3 scale;
	vec3 forward;
	vec3 up;
	vec3 right;
	int transformID;
	bool isSelectable;
	bool isSelected;
	bool isChangedInGUI;

	mutable mat4 oldModelMatrix;
	mutable quat oldRotation;
	mutable vec3 oldPosition;
	mutable vec3 oldEulerAngles;
	mutable vec3 oldScale;
	mutable bool initializedOldStuff;

	vector<Transform*> children;
	Transform* parent;
	GameObject* gameObject;
private:
	struct SelectedTransform {
		Transform* transform;
		RaycastHit hitInfo;
	};
	static map<int, Transform*> sm_transforms;
};

#endif //_TRANSFORM_H_