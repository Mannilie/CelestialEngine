#include "MaterialData.h"

const vec3* MaterialData::GetVector3(const string& _name) const {
	map<string, vec3>::const_iterator it = m_vec3Map.find(_name);
	if (it != m_vec3Map.end()) {
		return &it->second;
	}
	return &m_defaultVec3;
}

float* MaterialData::GetFloat(const string& name) const {
	map<string, float>::const_iterator it = m_floatMap.find(name);
	if (it != m_floatMap.end()) {
		return (float*)&it->second;
	}
	return 0;
}

const Texture* MaterialData::GetTexture(const string& name) const {
	map<string, Texture>::const_iterator it = m_textureMap.find(name);
	if (it != m_textureMap.end()) {
		return &it->second;
	}
	return &m_defaultTexture;
}
