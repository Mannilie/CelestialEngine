/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: RenderingEngine.h
@date: 14/06/2015
@author: Emmanuel Vaccaro
@brief: The rendering engine handles all types
of rendering within the game engine
===============================================*/

#ifndef _RENDERING_ENGINE_H_
#define _RENDERING_ENGINE_H_

// Objects
#include "Shader.h"

// Components
#include "Transform.h"
#include "Lighting.h"
#include "Camera.h"

// Structs
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Color.h"

// Utilities
#include "GLM_Header.h"
#include "MaterialData.h"
#include "Window.h"

// Other
#include <map>
using std::map;
#include <string>
using std::string;
#include <queue>
using std::queue;

struct DrawCommandMesh {
	string fileName;
	string materialName;
	Transform* transform;
	vector<Material>* materials;
	Camera* camera;
	Mesh* mesh;
	bool depthTestEnabled;
	bool wireframe;
};

struct RenderQueue {
	DrawCommandMesh* command;
	int commandCount; //size of command list
};

class RenderingEngine : public MaterialData {
public:
	RenderingEngine();
	virtual ~RenderingEngine() {}
	void AddDrawCommandMesh(DrawCommandMesh* _command);
	void OptimizeMeshRenderQueue(queue<DrawCommandMesh*>& _meshDrawCommands);
	void Render(vector<GameObject*>& _objects);
	void RenderAllObjects();
	void RenderAllDepthTestObjects();
	void SortRenderQueueByMaterial(queue<DrawCommandMesh*>& _meshDrawCommands);
	inline vector<DirectionalLight*>& GetActiveDirLights() { return m_dirLights; }
	inline vector<PointLight*>& GetActivePointLights() { return m_pointLights; }
	inline unsigned int GetSamplerSlot(const string& samplerName) const { return m_samplerMap.find(samplerName)->second; }
	inline const mat4& GetLightMatrix() const { return m_lightMatrix; }
	inline void SetSamplerSlot(const string& _name, unsigned int _value) { m_samplerMap[_name] = _value; }
	static void AddDirLight(DirectionalLight& _dirLight);
	static void AddPointLight(PointLight& _pointLight);
	virtual void UpdateUniformStruct(const Transform& transform,
		const Material& material,
		const Shader& shader,
		const string& uniformName,
		const string& uniformType) const;

	queue<DrawCommandMesh*> meshDrawCommands;
	queue<DrawCommandMesh*> depthQueue;
	
private:
	RenderingEngine(const RenderingEngine& other) : m_altCamera(mat4()) {}
	void operator=(const RenderingEngine& other) {}
	void BlurShadowMap(int shadowMapIndex, float blurAmount);
	void ApplyFilter(Shader& filter, const Texture& source, const Texture* dest);
	void DrawGrid(int _rows, int _cols, int _spacing);

	static const int NUM_SHADOW_MAPS = 1;
	static const mat4 BIAS_MATRIX;
	map<string, unsigned int> m_samplerMap;
	static vector<DirectionalLight*> m_dirLights;
	static vector<PointLight*> m_pointLights;
	Camera m_altCamera;
	Mesh m_plane;
	Material m_planeMaterial;
	Texture m_tempTarget;
	Texture m_shadowMaps[NUM_SHADOW_MAPS];
	Texture m_shadowMapTempTargets[NUM_SHADOW_MAPS];
	Shader m_defaultShader;
	Shader m_shadowMapShader;
	Shader m_nullFilter;
	Shader m_gausBlurFilter;
	Shader m_fxaaFilter;
	Shader m_lightingShader;
	Transform m_planeTransform;
	Transform m_altCameraTransform;
	mat4 m_lightMatrix;
	Color m_innerGridColor;
	Color m_outerGridColor;
	float m_fxaaSpanMax;
	float m_fxaaReduceMin;
	float m_fxaaReduceMul;
	float m_fxaaAspectDistortion;
};


#endif //_RENDERING_ENGINE_H_