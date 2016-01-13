/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Material.h
@date: 19/06/2015
@author: Emmanuel Vaccaro
@brief: Defines an object's material.
===============================================*/

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

// Utilities
#include "MaterialData.h"

// Other
#include <string>
using std::string;

class Material {
public:
	Material(const string& _materialName = "");
	virtual ~Material();
	Material(const string& _materialName, 
		const Texture& _diffuse,
		const Texture& _specular = Texture("default_specular.png"),
		float _specularIntensity = 0.0f,
		float _specularPower = 15.0f,
		const Texture& _normalMap = Texture("default_normal.jpg"),
		const Texture& _dispMap = Texture("default_displacement.png"),
		float _dispMapScale = 0.0f,
		float dispMapOffset = 0.0f);
	void SetVector3(const string& _name, const vec3& _value);
	void SetFloat(const string& _name, float _value);
	void SetTexture(const string& _name, const Texture& _value);
	const vec3* GetVector3(const string& _name) const;
	float* GetFloat(const string& _name) const;
	const Texture* GetTexture(const string& _name) const;
	void Inspector();
	static void Shutdown();

	string materialName;
	MaterialData* materialData;
private:
	static map<string, MaterialData*> sm_resourceMap;
};

#endif //_MATERIAL_H_