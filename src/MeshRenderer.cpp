#include "MeshRenderer.h"

// Objects
#include "GameObject.h"

// Components
#include "BoxCollider.h"

// GUI
#include "imgui.h"

MeshRenderer::MeshRenderer(const Mesh& _mesh, 
	const Material& _material, 
	bool _wireframe, 
	bool _depthTestEnabled) :
	mesh(_mesh),
	depthTestEnabled(_depthTestEnabled),
	wireframe(_wireframe) {
	materials.push_back(_material);
}
bool MeshRenderer::Startup() {
	if (mesh.model->materials.size() > 0) {
		materials = mesh.model->materials;
	}
	BoxCollider* boxCollider = gameObject->GetComponent<BoxCollider>();
	if (boxCollider != nullptr) {
		Bounds meshBounds = mesh.bounds;
		Bounds& colliderBounds = boxCollider->bounds;
		colliderBounds = meshBounds;
		colliderBounds.center = transform->position + meshBounds.center;
		colliderBounds.size = transform->scale * meshBounds.size;
		colliderBounds.min = colliderBounds.center - colliderBounds.size;
		colliderBounds.max = colliderBounds.center + colliderBounds.size;
	}

	return true;
}
bool MeshRenderer::Update() { 
	if (transform && transform->isSelected)	{ Inspector(); }
	bounds.center = transform->position + mesh.bounds.center;
	bounds.size = transform->scale * mesh.bounds.size;
	bounds.min = bounds.center - bounds.size;
	bounds.max = bounds.center + bounds.size;
	return true; 
}
void MeshRenderer::Inspector() {
	ImGui::Begin("Inspector");
	ImGui::BeginChild("Component", ImVec2(0, 0), true);
	if (ImGui::TreeNode("MeshRenderer")) {
		mesh.Inspector();
		ImGui::Checkbox("Wireframe", &wireframe);
		for (unsigned int i = 0; i < materials.size(); ++i) {
			materials[i].Inspector();
		}
		ImGui::TreePop();
	}
	ImGui::EndChild();
	ImGui::End();
}
void MeshRenderer::Draw(RenderingEngine& _renderer) {
	drawCommandMesh.transform = transform;
	drawCommandMesh.mesh = &mesh;
	drawCommandMesh.materials = &materials;
	drawCommandMesh.wireframe = wireframe;
	drawCommandMesh.depthTestEnabled = depthTestEnabled;
	drawCommandMesh.camera = Camera::current;
	_renderer.AddDrawCommandMesh(&drawCommandMesh);
}