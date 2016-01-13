#include "RenderingEngine.h"
#include "GameObject.h"

// GUI
#include "imgui.h"

// Debugging
#include "Debug.h"
#include "Gizmos.h"

// Other
#include <algorithm>

const mat4 RenderingEngine::BIAS_MATRIX = glm::scale(vec3(0.5, 0.5, 0.5)) * glm::translate(vec3(1.0, 1.0, 1.0));
// Construct a Matrix in this format:
//      x    y    z    w
// x [ 0.5, 0.0, 0.0, 0.5 ]
// y [ 0.0, 0.5, 0.0, 0.5 ]
// z [ 0.0, 0.0, 0.5, 0.5 ]
// w [ 0.0, 0.0, 0.0, 1.0 ]

// Note the 'w' column in this representation should be the translation column!
// This matrix will convert 3D coordinates from the range (-1, 1) to the range (0, 1).

// Create static directional lights
vector<DirectionalLight*>	RenderingEngine::m_dirLights;
vector<PointLight*>			RenderingEngine::m_pointLights;

// Public
RenderingEngine::RenderingEngine() :
	m_plane(Mesh("plane.obj")),
	m_tempTarget(Window::width, Window::height, 0, "", GL_TEXTURE_2D, GL_NEAREST, GL_RGBA, GL_RGBA, false, GL_COLOR_ATTACHMENT0),
	m_planeMaterial("renderingEngine_filterPlane", m_tempTarget, 1, 8),
	m_defaultShader("defaultShader"),
	m_shadowMapShader("shadowMapGenerator"),
	m_nullFilter("filter-null"),
	m_gausBlurFilter("filter-gausBlur7x1"),
	m_fxaaFilter("filter-fxaa"),
	m_lightingShader("default-forward-lighting"),
	m_altCameraTransform(vec3(0, 0, 0), quat(glm::radians(180.0f), vec3(0, 1, 0)), vec3(1)),
	m_altCamera(mat4(), &m_altCameraTransform),
	m_fxaaSpanMax(8.0f),
	m_fxaaReduceMin(1.0f / 128.0f),
	m_fxaaReduceMul(1.0f / 8.0f),
	m_fxaaAspectDistortion(150.0f) {

	SetSamplerSlot("diffuse", 0);
	SetSamplerSlot("normalMap", 1);
	SetSamplerSlot("dispMap", 2);
	SetSamplerSlot("shadowMap", 3);
	SetSamplerSlot("specMap", 4);

	SetSamplerSlot("filterTexture", 0);

	SetVector3("ambient", vec3(0.2f, 0.2f, 0.2f));

	SetFloat("fxaaSpanMax", 8.0f);
	SetFloat("fxaaReduceMin", 1.0f / 128.0f);
	SetFloat("fxaaReduceMul", 1.0f / 8.0f);
	SetFloat("fxaaAspectDistortion", 150.0f);

	SetTexture("displayTexture", Texture(Window::width, Window::height, 0, "displayTexture", GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);

	m_planeTransform.scale = vec3(1.0f);
	m_planeTransform.Rotate(vec3(-90, 0, 0));

	for (int i = 0; i < NUM_SHADOW_MAPS; i++) {
		int shadowMapSize = 1 << (i + 1);
		m_shadowMaps[i] = Texture(shadowMapSize, shadowMapSize, 0, "", GL_TEXTURE_2D, GL_LINEAR, GL_RG32F, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
		m_shadowMapTempTargets[i] = Texture(shadowMapSize, shadowMapSize, 0, "", GL_TEXTURE_2D, GL_LINEAR, GL_RG32F, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
	}

	m_lightMatrix = glm::scale(vec3(0, 0, 0));
	m_innerGridColor = Color(1, 1, 1, 25.0f / 255.0f);
	m_outerGridColor = Color(1, 1, 1, 100.0f / 255.0f);
}
void RenderingEngine::AddDrawCommandMesh(DrawCommandMesh* _command) {
	meshDrawCommands.push(_command);
}
void RenderingEngine::OptimizeMeshRenderQueue(queue<DrawCommandMesh*>& _meshDrawCommands) {
	// Note(Manny): Optimize mesh render queue to sort by game object renderer materials
}
void RenderingEngine::Render(vector<GameObject*>& _objects) {
	m_planeTransform.Update();

	ImGui::Begin("FXAA");
	ImGui::DragFloat("FXAA Span Max", &m_fxaaSpanMax);
	ImGui::DragFloat("FXAA Reduce Min", &m_fxaaReduceMin);
	ImGui::DragFloat("FXAA Reduce Mul", &m_fxaaReduceMul);
	ImGui::DragFloat("FXAA Aspect Distortion", &m_fxaaAspectDistortion);
	ImGui::End();

	SetFloat("fxaaSpanMax", m_fxaaSpanMax);
	SetFloat("fxaaReduceMin", m_fxaaReduceMin);
	SetFloat("fxaaReduceMul", m_fxaaReduceMul);
	SetFloat("fxaaAspectDistortion", m_fxaaAspectDistortion);

	SetFloat("shininess", 0.5f);

	GetTexture("displayTexture")->BindAsRenderTarget();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < _objects.size(); ++i) { _objects[i]->Draw(*this); }
	RenderAllObjects();

	float displayTextureAspect = (float)GetTexture("displayTexture")->GetWidth() / (float)GetTexture("displayTexture")->GetHeight();
	float displayTextureHeightAdditive = displayTextureAspect * *GetFloat("fxaaAspectDistortion");
	SetVector3("inverseFilterTextureSize", vec3(1.0f / (float)GetTexture("displayTexture")->GetWidth(), 1.0f / ((float)GetTexture("displayTexture")->GetHeight() + displayTextureHeightAdditive), 0.0f));

	DrawGrid(50, 50, 1);

	Gizmos::Draw(Camera::current->projectionMatrix * Camera::current->viewMatrix);

	RenderAllDepthTestObjects();

	ApplyFilter(m_fxaaFilter, *GetTexture("displayTexture"), 0);
}
void RenderingEngine::RenderAllObjects() {
	//SortRenderQueueByMaterial(meshDrawCommands);

	while (meshDrawCommands.size() > 0) {
		DrawCommandMesh* meshDrawCommand = meshDrawCommands.front();
		meshDrawCommands.pop();
		
		if (!meshDrawCommand->depthTestEnabled) {
			depthQueue.push(meshDrawCommand);
			continue;
		}

		Shader& shader = meshDrawCommand->mesh->shader;
		shader.Enable();
		shader.UpdateUniforms(*this);
		shader.UpdateCameraUniforms(*meshDrawCommand->camera);

		//Note(Manny): Only update uniforms once for each material
		
		shader.UpdateTransformUniforms(*meshDrawCommand->transform);

		Mesh* mesh = meshDrawCommand->mesh;
		IndexedModel* model = mesh->model;
		vector<MeshData>& meshes = model->meshes;
		vector<Material>& materials = *meshDrawCommand->materials;
		
		unsigned int materialIndex = 0; 
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			shader.UpdateMaterialUniforms(materials[materialIndex], *this);

			materialIndex++;
			
			if (materialIndex >= materials.size()) {
				materialIndex--;
			}

			mesh->UpdateAllBones();
			mesh->DrawGizmosBones();

			if (model->skeletons.size() > 0) {
				shader.SetMatrix4("bones", model->skeletons[0]->m_boneCount, *model->skeletons[0]->m_bones, GL_FALSE);
			}

			if (meshDrawCommand->wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glBindVertexArray(meshes[i].glData.VAO);
			glDrawElements(GL_TRIANGLES, meshes[i].glData.indexCount, GL_UNSIGNED_INT, 0);
			
			if (meshDrawCommand->wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
}
void RenderingEngine::RenderAllDepthTestObjects() {
	while (depthQueue.size() > 0) {
		glClear(GL_DEPTH_BUFFER_BIT);

		DrawCommandMesh* meshDrawCommand = depthQueue.front();
		depthQueue.pop();

		Shader& shader = meshDrawCommand->mesh->shader;
		shader.Enable();
		shader.UpdateUniforms(*this);
		shader.UpdateCameraUniforms(*meshDrawCommand->camera);

		//Note(Manny): Only update uniforms once for each material
		
		shader.UpdateTransformUniforms(*meshDrawCommand->transform);

		Mesh* mesh = meshDrawCommand->mesh;
		IndexedModel* model = mesh->model;
		vector<MeshData>& meshes = model->meshes;
		vector<Material>& materials = *meshDrawCommand->materials;

		unsigned int materialIndex = 0;
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			shader.UpdateMaterialUniforms(materials[materialIndex], *this);

			materialIndex++;

			if (materialIndex >= materials.size()) {
				materialIndex--;
			}

			mesh->UpdateAllBones();
			mesh->DrawGizmosBones();

			if (model->skeletons.size() > 0) {
				shader.SetMatrix4("bones", model->skeletons[0]->m_boneCount, *model->skeletons[0]->m_bones, GL_FALSE);
			}

			if (meshDrawCommand->wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			glBindVertexArray(meshes[i].glData.VAO);
			glDrawElements(GL_TRIANGLES, meshes[i].glData.indexCount, GL_UNSIGNED_INT, 0);

			if (meshDrawCommand->wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
}
void RenderingEngine::SortRenderQueueByMaterial(queue<DrawCommandMesh*>& _meshDrawCommands)
{
	//Note(Manny): Sort by material here
	
}

// Static
void RenderingEngine::AddDirLight(DirectionalLight& _dirLight) { m_dirLights.push_back(&_dirLight); }
void RenderingEngine::AddPointLight(PointLight& _pointLight) { m_pointLights.push_back(&_pointLight); }

// Virtual 
void RenderingEngine::UpdateUniformStruct(const Transform& transform,
	const Material& material,
	const Shader& shader,
	const string& uniformName,
	const string& uniformType) const {
	throw uniformType + " is not supported by the rendering engine";
}

// Private
void RenderingEngine::BlurShadowMap(int shadowMapIndex, float blurAmount) {
	SetVector3("blurScale", vec3(blurAmount / (m_shadowMaps[shadowMapIndex].GetWidth()), 0.0f, 0.0f));
	ApplyFilter(m_gausBlurFilter, m_shadowMaps[shadowMapIndex], &m_shadowMapTempTargets[shadowMapIndex]);
	SetVector3("blurScale", vec3(0.0f, blurAmount / (m_shadowMaps[shadowMapIndex].GetHeight()), 0.0f));
	ApplyFilter(m_gausBlurFilter, m_shadowMapTempTargets[shadowMapIndex], &m_shadowMaps[shadowMapIndex]);
}
void RenderingEngine::ApplyFilter(Shader& filter, const Texture& source, const Texture* dest) {
	assert(&source != dest);
	if (dest == 0) {
		Window::BindAsRenderTarget();
	} else {
		dest->BindAsRenderTarget();
	}

	m_altCamera.projectionMatrix = mat4();
	m_altCamera.transform->position = vec3(0, 0, 0);
	m_altCamera.transform->rotation = quat(glm::radians(180.0f), vec3(1, 0, 0));

	SetTexture("filterTexture", source);

	glClear(GL_DEPTH_BUFFER_BIT);
	filter.Enable();
	filter.UpdateUniforms(*this);
	filter.UpdateTransformUniforms(m_planeTransform);
	filter.UpdateMaterialUniforms(m_planeMaterial, *this);
	filter.UpdateCameraUniforms(m_altCamera);
	m_plane.Draw(*this);

	SetTexture("filterTexture", 0);
}
void RenderingEngine::DrawGrid(int _rows, int _cols, int _spacing) {
	Gizmos::AddTransform(mat4(1));
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 blue(0, 0, 1, 1);
	int maxLines = _rows + _cols;
	int rows = _rows * _spacing;
	int cols = _cols * _spacing;
	for (int line = 0; line <= maxLines; ++line) {
		int lineOffset = line * _spacing;
		Gizmos::AddLine(vec3(-rows + lineOffset, 0, -rows),
			vec3(-rows + lineOffset, 0, rows), 
			line % 10 == 0 ? m_outerGridColor.ToVec4() : m_innerGridColor.ToVec4());
		Gizmos::AddLine(vec3(-cols, 0, -cols + lineOffset), 
			vec3(cols, 0, -cols + lineOffset), 
			line % 10 == 0 ? m_outerGridColor.ToVec4() : m_innerGridColor.ToVec4());
	}
}
