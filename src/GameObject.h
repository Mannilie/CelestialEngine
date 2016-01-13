/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: GameObject.h
@date: 14/06/2015
@author: Emmanuel Vaccaro
@brief: The rendering engine handles all types
of rendering within the game engine
===============================================*/

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

// Utilities
#include "GLM_Header.h"

// Objects
#include "Object.h"

// Components
#include "Transform.h"
#include "MeshRenderer.h"
#include "Collider.h"

// Other
#include <iostream>
#include <map>
using std::map;
#include <vector>
using std::vector;

// Forward declaration
class Component;

class GameObject : public Object {
public:
	GameObject(string _name = "GameObject");
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw(RenderingEngine& _renderer);
	template<typename T>
	T* AddComponent(Component& _component = T());
	vector<Component*> components;
	Collider* AddCollider(Collider* _collider);
	vector<Collider*> colliders;
	template<typename T>
	T* GetComponent();

	Transform transform; 
	bool isStatic;
private:
	bool isVisible; //Is the object visible in the scene?
};
template<typename T>
T* GameObject::AddComponent(Component& _component) {
	T* componentCheck;
	Collider* colliderCheck = dynamic_cast<Collider*>(&_component);
	if (colliderCheck == nullptr) {
		for (unsigned int i = 0; i < components.size(); ++i) {
			componentCheck = dynamic_cast<T*>(components[i]);
			if (componentCheck != nullptr) {
				std::cout << "Error: Cannot attach multiples of the same " <<
					"component to the same GameObject! GameObject Name: " <<
					this->name << std::endl;
				return nullptr;
			}
		}
	}
	T* newComponent = new T(dynamic_cast<T&>(_component));
	newComponent->gameObject = this;
	newComponent->transform = &transform;
	newComponent->transform->gameObject = this;
	components.push_back(newComponent);
	return newComponent;
}
template<typename T>
T* GameObject::GetComponent() {
	for (unsigned int i = 0; i < components.size(); ++i) {
		T* component = dynamic_cast<T*>(components[i]);
		if (component != nullptr) {
			return component;
		}
	}
	return nullptr;
}

#endif // _GAMEOBJECT_H_