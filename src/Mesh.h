/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Mesh.h
@date: 18/06/2015
@author: Emmanuel Vaccaro
@brief: A Mesh contains vertices and 
multiple triangle arrays.
===============================================*/

#ifndef _MESH_H_
#define _MESH_H_

// Objects
#include "Shader.h"

// Utilities
#include "GLFW_Header.h"
#include "GLM_Header.h"

// Structs
#include "Vertex.h"
#include "Bounds.h"
#include "Texture.h"
#include "Material.h"

// Other
#include <FBXFile.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;

// Forward declaration
class RenderingEngine;

class MeshData
{
public:
	MeshData(){}
	~MeshData();
	void AddVertex(const vec3& _vert);
	void AddTexCoord(const vec2& _texCoord);
	void AddBoneIndices(const vec4& _boneIndices);
	void AddBoneWeights(const vec4& _boneWeights);
	void AddNormal(const vec3& _normal);
	void AddTangent(const vec3& _tangent);
	void AddIndices(const GLuint& _indices);
	void AddFace(GLuint _vertIndex0, GLuint _vertIndex1, GLuint _vertIndex2);
	void CalculateNormals();
	void CalculateTangents();
	MeshData Finalize();
	bool IsValid() const;

	vector<vec3> positions;
	vector<vec2> texCoords;
	vector<vec4> boneIndices;
	vector<vec4> boneWeights;
	vector<vec3> normals;
	vector<vec3> tangents;
	vector<GLuint> indices;
	OpenGLData glData;
};

class IndexedModel {
public:
	IndexedModel(){}
	virtual ~IndexedModel(){}
	void Init();
	void Draw(RenderingEngine& _renderer);
	void Finalize();
	void AddSkeleton(FBXSkeleton* _skeleton);
	void AddAnimation(FBXAnimation* _animation);

	vector<FBXSkeleton*> skeletons;
	vector<FBXAnimation*> animations;
	vector<MeshData> meshes;
	vector<Material> materials;
};

struct FBXTexture;
class Mesh {
public:
	Mesh(const string& _fileName = "cube.obj", bool _useDefaultDir = true);
	Mesh(const string& _fileName, const Shader& _shader, bool _useDefaultDir = true);
	virtual ~Mesh();
	void Init(const string& _fileName, bool _useDefaultDir);
	void Draw(RenderingEngine& _renderer);
	void Inspector();
	void LoadOBJFile(const string& _fileName);
	void LoadFBXFile(const string& _fileName);
	Texture CreateFBXTexture(FBXTexture* _fbxTexture);
	Bounds CalculateMeshBounds();
	void UpdateAllBones();
	void DrawGizmosBones();
	static void Shutdown();

	Shader shader;
	Bounds bounds;
	string fileName;
	bool loadFBXTextures;
	IndexedModel* model;
private:
	static map<string, IndexedModel*> sm_resourceMap;
};


#endif // _MESH_H_