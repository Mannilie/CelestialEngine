/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Shader.h
@date: 16/06/2015
@author: Emmanuel Vaccaro
@brief: Defines shaders within the engine
===============================================*/

#ifndef _SHADER_H_
#define _SHADER_H_

// Objects
#include "Object.h"

// Utilities
#include "GLFW_Header.h"
#include "GLM_Header.h"


// Other
#include <string>
using std::string;
using std::to_string;
#include <map>
using std::map;
#include <vector>
using std::vector;
using std::pair;

// Forward declaration
class RenderingEngine;
class Material;
class Transform;
class Color;
class DirectionalLight;
class PointLight;
class SpotLight;

enum ShaderType {
	SHADER_NULL,
	SHADER_VERTEX,
	SHADER_FRAGMENT,
	SHADER_GEOMETRY,
	SHADER_TESSELATION
};

struct TypedData {
	TypedData(const string& _name, const string& _type) :
		name(_name),
		type(_type) {}
	string name;
	string type;
};

struct UniformData {
public:
	UniformData(const string& _name, const vector<TypedData>& _memberNames) :
		name(_name),
		memberNames(_memberNames) {}

	string            name;
	vector<TypedData> memberNames;
};

class ShaderData {
public:
	ShaderData(const string& _fileName);
	virtual ~ShaderData() {}
	void LoadFromFile(const string& _file, ShaderType _shaderType);
	void AddVertexShader(const string& _text);
	void AddGeometryShader(const string& _text);
	void AddFragmentShader(const string& _text);
	bool AddProgram(const string& _text, int _type);
	void AddAllAttributes(const string& _vertexShaderText, const string& _attributeKeyword);
	void AddShaderUniforms(const string& _shaderText);
	void AddUniform(const string& _uniformName, const string& _uniformType, const vector<UniformData>& _structs);
	void CompileShader();
	void GetAllUniforms();
	GLint GetLocation(string _name);
	
	static int s_supportedOpenGLLevel;
	static string s_glslVersion;
	string fileName;
	int program;
	vector<int> shaders;
	vector<string> uniformNames;
	vector<string> uniformTypes;
	map<string, GLint> uniformMap;
private:
	bool CompileShader(string _file, GLuint& _shaderHandle);
	bool CompileSucceeded(GLuint _shaderHandle);
	bool LinkSucceeded() const;
};

class Shader {
public:
	static int shaderCount; //Number of all shaders created
	static Shader* Find(string _name);
	static Shader* FindShader(vector<string>& _keywords);
	static void WarmupAllShaders(); //Fully loads all shaders
	Shader(string _fileName = "basicShader");
	~Shader();
	void UpdateUniforms(RenderingEngine& _renderer);
	void UpdateTransformUniforms(const Transform& _transform);
	void UpdateCameraUniforms(const Camera& _camera);
	void UpdateMaterialUniforms(const Material& _material, RenderingEngine& _renderer);
	void Enable() const;
	void Disable() const;
	void SetFloat(string _propertyName, const float& _value);
	void SetInt(string _propertyName, const int& _value);
	void SetMatrix4(string _propertyName, const int _size, const mat4& _value, bool _transposed = false);
	void SetVector3(string _propertyName, const vec3& _value);
	void SetVector3(string _propertyName, const Color& _value);
	void SetVector4(string _propertyName, const vec4& _value);
	void SetFloat2(string _propertyName, const vec2& _value);
	void SetFloat2(string _propertyName, const float& _a, const float& _b);
	void SetFloat3(string _propertyName, const vec3& _value);
	void SetFloat3(string _propertyName, const float& _a, const float& _b, const float& _c);
	void SetDirectionalLights(const string& _uniformName, const vector<DirectionalLight*>& _dirLights);
	void SetPointLights(const string& _uniformName, const vector<PointLight*>& _pointLights);
	
	static void Shutdown();

	bool isSupported; //Can this shader run on the end-users graphics card?
	int renderQueue;
	string fileName;
	ShaderData* shaderData;
private:
	static map<string, ShaderData*> sm_resourceMap;
};

#endif // _SHADER_H_