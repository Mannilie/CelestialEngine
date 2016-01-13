/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: MaterialData.h
@date: 23/06/2015
@author: Emmanuel Vaccaro
@brief: A container for the material data
===============================================*/

#ifndef _MATERIAL_DATA_H_
#define _MATERIAL_DATA_H_

// Structs
#include "Texture.h"

// Utilities
#include "GLM_Header.h"

class MaterialData
{
public:
	MaterialData() :
		m_defaultTexture(Texture("default_texture.png")),
		m_defaultVec3(vec3(0, 0, 0)) {}
	inline void SetVector3(const string& _name, const vec3& _value)	{ m_vec3Map[_name] = _value; }
	inline void SetFloat(const string& _name, float _value) { m_floatMap[_name] = _value; }
	inline void SetTexture(const string& _name, const Texture& _value) { m_textureMap[_name] = _value; }
	const vec3* GetVector3(const string& _name)  const;
	float* GetFloat(const string& _name) const;
	const Texture* GetTexture(const string& _name) const;

private:
	map<string, vec3> m_vec3Map;
	map<string, float> m_floatMap;
	map<string, Texture> m_textureMap;

	Texture m_defaultTexture;
	vec3 m_defaultVec3;
};

#endif // _MATERIAL_DATA_H_