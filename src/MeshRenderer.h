/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: MeshRenderer.h
@date: 01/07/2015
@author: Emmanuel Vaccaro
@brief: Renders meshes in the scene
===============================================*/

#ifndef _MESH_RENDERER_H_
#define _MESH_RENDERER_H_

// Sub-engines
#include "RenderingEngine.h"

// Objects
#include "Shader.h"

// Components
#include "Component.h"

// Structs
#include "Mesh.h"
#include "Material.h"
#include "Bounds.h"

class MeshRenderer : public Component {
public:
	MeshRenderer(const Mesh& _mesh, const Material& _material = Material(), bool _wireframe = false, bool _depthTestEnabled = true);
	virtual bool Startup();
	virtual void Shutdown(){}
	virtual bool Update();
	virtual void Draw(RenderingEngine& _renderer);
	void Render();
	void Inspector();

	vector<Material> materials;
	Mesh mesh;
	DrawCommandMesh drawCommandMesh;
	Bounds bounds;
	bool wireframe;
	bool depthTestEnabled;
};

#endif // _MESH_RENDERER_H_