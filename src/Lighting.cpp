#include "Lighting.h"

// Sub-engines
#include "RenderingEngine.h"

// Componhents
#include "Transform.h"

// GUI
#include "imgui.h"

// Directional Light
DirectionalLight::DirectionalLight(vec3 _direction,
	const vec3& _ambient, const vec3& _diffuse, const vec3& _specular) :
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular),	
	direction(_direction) {}
bool DirectionalLight::Startup() {
	RenderingEngine::AddDirLight(*this);
	return true;
}
bool DirectionalLight::Update() {
	if (transform && transform->isSelected) { Inspector(); }
	return true;
}
void DirectionalLight::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("DirectionalLight")) {
		// Settings go here
		ImGui::ColorEdit3("Ambient", (float*)&ambient);
		ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
		ImGui::ColorEdit3("Specular", (float*)&specular);
		ImGui::DragFloat3("Direction", (float*)&direction, 0.01f, -1.0f, 1.0f);
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}

// PointLight
PointLight::PointLight(const Attenuation& _attenuation,
	const vec3& _ambient, const vec3& _diffuse, const vec3& _specular) : 
	ambient(_ambient),
	diffuse(_diffuse),
	specular(_specular),
	attenuation(_attenuation) {}
bool PointLight::Startup() {
	RenderingEngine::AddPointLight(*this);
	return true;
}
bool PointLight::Update() {
	if (transform && transform->isSelected) { Inspector(); }
	return true;
}
void PointLight::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("PointLight")) {
		// Settings go here
		ImGui::ColorEdit3("Ambient", (float*)&ambient);
		ImGui::ColorEdit3("Diffuse", (float*)&diffuse);
		ImGui::ColorEdit3("Specular", (float*)&specular);
		ImGui::DragFloat("Constant", &attenuation.constant, 0.01f);
		ImGui::DragFloat("Linear", &attenuation.linear, 0.01f);
		ImGui::DragFloat("Quadratic", &attenuation.quadratic, 0.01f);
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}