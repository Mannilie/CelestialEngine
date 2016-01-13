#include "Material.h"

// GUI
#include "imgui.h"

// Other
#include <iostream>

map<string, MaterialData*> Material::sm_resourceMap;

// Public
Material::Material(const string& _materialName) :
	materialName(_materialName) {
	if (_materialName.length() > 0) {
		map<string, MaterialData*>::const_iterator it = sm_resourceMap.find(_materialName);
		if (it == sm_resourceMap.end()) {
			std::cerr << "Error: Material " << _materialName << " has not been initialized!" << std::endl;
		}
		materialData = it->second;
	}
}
Material::~Material(){}
Material::Material(const string& _materialName, 
	const Texture& _diffuse,
	const Texture& _specular,
	float _specularIntensity,
	float _specularPower,
	const Texture& _normalMap,
	const Texture& _dispMap, 
	float _dispMapScale,
	float dispMapOffset) : 
	materialName(_materialName) {
	materialData = new MaterialData();
	sm_resourceMap[materialName] = materialData;
	materialData->SetTexture("diffuse", _diffuse);
	materialData->SetTexture("specMap", _specular);
	materialData->SetFloat("specularIntensity", _specularIntensity);
	materialData->SetFloat("specularPower", _specularPower);
	materialData->SetTexture("normalMap", _normalMap);
	materialData->SetTexture("dispMap", _dispMap);
	float baseBias = _dispMapScale / 2.0f;
	materialData->SetFloat("dispMapScale", _dispMapScale);
	materialData->SetFloat("dispMapBias", -baseBias + baseBias * dispMapOffset);
}
void Material::SetVector3(const string& _name, const vec3& _value) {
	materialData->SetVector3(_name, _value);
}
void Material::SetFloat(const string& _name, float _value) {
	materialData->SetFloat(_name, _value);
}
void Material::SetTexture(const string& _name, const Texture& _value) {
	materialData->SetTexture(_name, _value);
}
const vec3* Material::GetVector3(const string& _name) const {
	return materialData->GetVector3(_name);
}
float* Material::GetFloat(const string& _name) const {
	return materialData->GetFloat(_name);
} 
const Texture* Material::GetTexture(const string& _name) const {
	return materialData->GetTexture(_name);
}
void Material::Inspector() {
	if (ImGui::TreeNode("Material")) {
		ImGui::DragFloat("Specular Intensity", materialData->GetFloat("specularIntensity"));
		ImGui::DragFloat("Specular Power", materialData->GetFloat("specularPower"));
		ImGui::TreePop();
	}
}

// Static
void Material::Shutdown() {
	for (auto resource : sm_resourceMap) {
		delete resource.second;
	}
	sm_resourceMap.clear();
}