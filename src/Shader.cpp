#include "Shader.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "RenderingEngine.h"
#include "Transform.h"
#include "Camera.h"
#include "Debug.h"

map<string, ShaderData*> Shader::sm_resourceMap;
int ShaderData::s_supportedOpenGLLevel = 0;
string ShaderData::s_glslVersion = "";

// Static
static void CheckShaderError(int _shader, int _flag, bool _isProgram, const string& _errorMessage);
static vector<UniformData> FindUniformStructs(const string& _shaderText);
static string FindUniformStructName(const string& _structStartToOpeningBrace);
static vector<TypedData> FindUniformStructComponents(const string& _openingBraceToClosingBrace);
static string LoadShader(const string& _fileName);

// ShaderData
ShaderData::ShaderData(const string& _fileName) {
	fileName = _fileName;

	program = glCreateProgram();

	if (program == 0) {
		Debug::LogError("Shader program error. File '" + fileName + " Error: Cannot create the shader program");
		return;
	}

	if (s_supportedOpenGLLevel == 0) {
		int majorVersion;
		int minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		s_supportedOpenGLLevel = majorVersion * 100 + minorVersion * 10;

		if (s_supportedOpenGLLevel >= 330) {
			std::ostringstream convert;
			convert << s_supportedOpenGLLevel;
			s_glslVersion = convert.str();
		} else if (s_supportedOpenGLLevel >= 320) {
			s_glslVersion = "150";
		} else if (s_supportedOpenGLLevel >= 310) {
			s_glslVersion = "140";
		} else if (s_supportedOpenGLLevel >= 300) {
			s_glslVersion = "130";
		} else if (s_supportedOpenGLLevel >= 210) {
			s_glslVersion = "120";
		} else if (s_supportedOpenGLLevel >= 200) {
			s_glslVersion = "110";
		} else {
			fprintf(stderr, "Error: OpenGL Version %d.%d does not support shaders.\n", majorVersion, minorVersion);
			exit(1);
		}
	}

	string shaderText = LoadShader(fileName + ".glsl");
	
	string vertexShaderText = "#version " + s_glslVersion + "\n#define VS_BUILD\n#define GLSL_VERSION " + s_glslVersion + "\n" + shaderText;
	string fragmentShaderText = "#version " + s_glslVersion + "\n#define FS_BUILD\n#define GLSL_VERSION " + s_glslVersion + "\n" + shaderText;

	AddVertexShader(vertexShaderText);
	AddFragmentShader(fragmentShaderText);
	
	string attributeKeyword = "uniform";
	AddAllAttributes(vertexShaderText, attributeKeyword);

	CompileShader();

	AddShaderUniforms(shaderText);
}
void ShaderData::LoadFromFile(const string& _file, ShaderType _shaderType) {
	GLuint shaderHandle = 0;
	//Create shader object
	switch (_shaderType) {
		case SHADER_VERTEX: { shaderHandle = glCreateShader(GL_VERTEX_SHADER); break; }
		case SHADER_FRAGMENT: { shaderHandle = glCreateShader(GL_FRAGMENT_SHADER); break; }
		case SHADER_GEOMETRY: { shaderHandle = glCreateShader(GL_GEOMETRY_SHADER); break; }
		case SHADER_TESSELATION: { shaderHandle = 0; std::cerr << "Tesselation shader is not yet supported..." << std::endl; }
	}

	if (CompileShader(_file, shaderHandle)) {
		shaders[_shaderType] = shaderHandle;
	} else {
		Debug::LogError("Loading the shader" + _file + "has failed! Line: " + to_string(__LINE__));
		glDeleteShader(shaderHandle);
	}
}
void ShaderData::AddVertexShader(const string& _text) {
	AddProgram(_text, GL_VERTEX_SHADER);
}
void ShaderData::AddGeometryShader(const string& _text) {
	AddProgram(_text, GL_GEOMETRY_SHADER);
}
void ShaderData::AddFragmentShader(const string& _text) {
	AddProgram(_text, GL_FRAGMENT_SHADER);
}
bool ShaderData::AddProgram(const string& _text, int _type) {
	int shader = glCreateShader(_type);
	
	if (shader == 0) {
		Debug::LogError("Shader load error. Shader file '" + fileName);
		return false;
	}

	const GLchar* p[1];
	p[0] = _text.c_str();
	GLint lengths[1];
	lengths[0] = _text.length();

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		Debug::LogError("Shader compile error. Shader file '" + fileName + " Error: " + infoLog);
		return false;
	}

	glAttachShader(program, shader);
	shaders.push_back(shader);

	return true;
}
void ShaderData::AddAllAttributes(const string& _vertexShaderText, const string& _attributeKeyword) {
	int currentAttribLocation = 0;
	size_t attributeLocation = _vertexShaderText.find(_attributeKeyword);
	while (attributeLocation != string::npos) {
		bool isCommented = false;
		size_t lastLineEnd = _vertexShaderText.rfind("\n", attributeLocation);

		if (lastLineEnd != string::npos) {
			string potentialCommentSection = _vertexShaderText.substr(lastLineEnd, attributeLocation - lastLineEnd);

			//Potential false positives are both in comments, and in macros.
			isCommented = potentialCommentSection.find("//") != string::npos ||
				potentialCommentSection.find("#") != string::npos;
		}
		if (!isCommented) {
			size_t begin = attributeLocation + _attributeKeyword.length();
			size_t end = _vertexShaderText.find(";", begin);

			string attributeLine = _vertexShaderText.substr(begin + 1, end - begin - 1);

			begin = attributeLine.find(" ");
			string attributeName = attributeLine.substr(begin + 1);

			glBindAttribLocation(program, currentAttribLocation, attributeName.c_str());
			currentAttribLocation++;
		}
		attributeLocation = _vertexShaderText.find(_attributeKeyword, attributeLocation + _attributeKeyword.length());
	}
}
void ShaderData::AddShaderUniforms(const string& _shaderText) {
	static const string UNIFORM_KEY = "uniform";

	vector<UniformData> structs = FindUniformStructs(_shaderText);

	size_t uniformLocation = _shaderText.find(UNIFORM_KEY);
	while (uniformLocation != string::npos) {
		bool isCommented = false;
		size_t lastLineEnd = _shaderText.rfind("\n", uniformLocation);

		if (lastLineEnd != string::npos) {
			string potentialCommentSection = _shaderText.substr(lastLineEnd, uniformLocation - lastLineEnd);
			isCommented = potentialCommentSection.find("//") != string::npos;
		}

		if (!isCommented) {
			size_t begin = uniformLocation + UNIFORM_KEY.length();
			size_t end = _shaderText.find(";", begin);

			string uniformLine = _shaderText.substr(begin + 1, end - begin - 1);

			begin = uniformLine.find(" ");
			string uniformName = uniformLine.substr(begin + 1);
			string uniformType = uniformLine.substr(0, begin);

			size_t subscriptLocation = uniformName.find("[");
			if (subscriptLocation != string::npos) {
				uniformName = uniformName.substr(0, subscriptLocation);
			}

			uniformNames.push_back(uniformName);
			uniformTypes.push_back(uniformType);
			AddUniform(uniformName, uniformType, structs);
		}
		uniformLocation = _shaderText.find(UNIFORM_KEY, uniformLocation + UNIFORM_KEY.length());
	}
}
void ShaderData::AddUniform(const string& _uniformName, const string& _uniformType, const vector<UniformData>& _structs) {
	bool addThis = true;

	for (unsigned int i = 0; i < _structs.size(); i++) {
		if (_structs[i].name.compare(_uniformType) == 0) {
			addThis = false;
			for (unsigned int j = 0; j < _structs[i].memberNames.size(); j++) {
				AddUniform(_uniformName + "." + _structs[i].memberNames[j].name, _structs[i].memberNames[j].type, _structs);
			}
		}
	} 
	if (!addThis) {
		return;
	}
	unsigned int location = glGetUniformLocation(program, _uniformName.c_str());
	uniformMap.insert(pair<string, unsigned int>(_uniformName, location));
}
void ShaderData::CompileShader() {
	glLinkProgram(program);
	CheckShaderError(program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(program);
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Invalid shader program");
}
bool ShaderData::CompileShader(string _file, GLuint& _shaderHandle) {
	bool successful = true;

	//Load the shader file
	FILE* shaderFile = fopen(_file.c_str(), "r");

	//Check if shader file loaded successfully
	if (shaderFile == 0) {
		//Close the file
		fclose(shaderFile);
	} else {
		//Find out how long the file is
		fseek(shaderFile, 0, SEEK_END);
		int shaderFileLength = ftell(shaderFile);
		fseek(shaderFile, 0, SEEK_SET);

		//Allocate enough space for the file
		char* shaderSource = new char[shaderFileLength];

		//Read the file and update the length to be accurate
		shaderFileLength = fread(shaderSource, 1, shaderFileLength, shaderFile);

		//Compile the shader to the handle that got passed in
		glShaderSource(_shaderHandle, 1, &shaderSource, &shaderFileLength);
		glCompileShader(_shaderHandle);

		//Check the shader for errors
		if (!CompileSucceeded(_shaderHandle)) {
			successful = false;
		}

		//Cleanup all that we allocated
		delete[] shaderSource;
		fclose(shaderFile);
	}
	return successful;
}
bool ShaderData::CompileSucceeded(GLuint _shaderHandle) {
	//Error checking
	int success = GL_FALSE;
	glGetShaderiv(_shaderHandle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(_shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength <= 0) {
			return false;
		}
		string infoLog;
		glGetShaderInfoLog(_shaderHandle, infoLogLength, &infoLogLength, &infoLog[0]);
		Debug::LogError("Shader compile error. Line '" + to_string(__LINE__) + " Error: " + infoLog);
		return false;
	}
	return true;
}
void ShaderData::GetAllUniforms() {
	int totalUniforms = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &totalUniforms);
	for (int i = 0; i < totalUniforms; ++i) {
		int nameLength = -1;
		int num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(program, GLuint(i), sizeof(name) - 1,
			&nameLength, &num, &type, name);
		name[nameLength] = 0;
		uniformMap[name] = glGetUniformLocation(program, name);
	}
}
GLint ShaderData::GetLocation(string _name) {
	if (uniformMap.find(_name) != uniformMap.end()) {
		return uniformMap[_name];
	}
	return -1;
}
void Shader::UpdateUniforms(RenderingEngine& _renderingEngine) {
	for (unsigned int i = 0; i < shaderData->uniformNames.size(); i++) {
		string uniformName = shaderData->uniformNames[i];
		string uniformType = shaderData->uniformTypes[i];

		if (uniformName.substr(0, 2) == "R_") {
			string unprefixedName = uniformName.substr(2, uniformName.length());

			if (unprefixedName == "lightMatrix") {
				SetMatrix4(uniformName, 1, _renderingEngine.GetLightMatrix());
			} else if (uniformType == "sampler2D") {
				int samplerSlot = _renderingEngine.GetSamplerSlot(unprefixedName);
				_renderingEngine.GetTexture(unprefixedName)->Bind(samplerSlot);
				SetInt(uniformName, samplerSlot);
			} else if (uniformType == "vec3") {
				SetVector3(uniformName, *_renderingEngine.GetVector3(unprefixedName));
			} else if (uniformType == "float") {
				SetFloat(uniformName, *_renderingEngine.GetFloat(unprefixedName));
			} else if (uniformType == "int") {
				if (uniformName == "R_DIR_LIGHT_COUNT") {
					SetInt(uniformName, _renderingEngine.GetActiveDirLights().size());
				} if (uniformName == "R_POINT_LIGHT_COUNT") {
					SetInt(uniformName, _renderingEngine.GetActivePointLights().size());
				}
			} else if (uniformType == "DirLight") {
				SetDirectionalLights(uniformName, _renderingEngine.GetActiveDirLights());
			} else if (uniformType == "PointLight") {
				SetPointLights(uniformName, _renderingEngine.GetActivePointLights());
			} else {
				throw "Invalid R_ Uniform:" + uniformName;
			}
		}
	}
}
void Shader::UpdateTransformUniforms(const Transform& _transform) {
	mat4 worldMatrix = _transform.worldMatrix;
	for (unsigned int i = 0; i < shaderData->uniformNames.size(); i++) {
		string uniformName = shaderData->uniformNames[i];
		string uniformType = shaderData->uniformTypes[i];
		if (uniformName.substr(0, 2) == "T_") {
			if (uniformName == "T_model") {
				SetMatrix4(uniformName, 1, worldMatrix);
			} else {
				throw "Invalid Transform Uniform: " + uniformName;
			}
		}
	}
}
void Shader::UpdateCameraUniforms(const Camera& _camera) {
	mat4 viewProj = _camera.projectionMatrix * _camera.viewMatrix;
	for (unsigned int i = 0; i < shaderData->uniformNames.size(); i++) {
		string uniformName = shaderData->uniformNames[i];
		string uniformType = shaderData->uniformTypes[i];

		if (uniformName.substr(0, 2) == "C_") {
			//uniformName = uniformName.substr(2, uniformName.length());
			if (uniformName == "C_eyePos") {
				vec3 cameraPos = _camera.transform->position;
				if (_camera.transform->parent) {
					cameraPos += _camera.transform->parent->position;
				}
				SetVector3(uniformName, cameraPos);
			} else if (uniformName == "C_viewProj") {
				SetMatrix4(uniformName, 1, viewProj);
			} else {
				throw "Invalid Camera Uniform: " + uniformName;
			}
		}
	}
}
void Shader::UpdateMaterialUniforms(const Material& _material, RenderingEngine& _renderer) {
	for (unsigned int i = 0; i < shaderData->uniformNames.size(); i++) {
		string uniformName = shaderData->uniformNames[i];
		string uniformType = shaderData->uniformTypes[i];
		string uniformPrefix = uniformName.substr(0, 2);
		if (uniformPrefix == "M_") {
			string unprefixedName = uniformName.substr(2, uniformName.length());
			if (uniformType == "sampler2D") {
				int samplerSlot = _renderer.GetSamplerSlot(unprefixedName);
				_material.GetTexture(unprefixedName)->Bind(samplerSlot);
				SetInt(uniformName, samplerSlot);
			} else if (uniformType == "vec3") {
				SetVector3(uniformName, *_material.GetVector3(unprefixedName));
			} else if (uniformType == "float") {
				SetFloat(uniformName, *_material.GetFloat(unprefixedName));
			} else {
				throw uniformType + " is not supported by the Material class";
			}
		}
	}
}
bool ShaderData::LinkSucceeded() const {
	//Error checking
	int success = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength <= 0) {
			return false;
		}
		std::vector<GLchar> infoLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, &infoLogLength, &infoLog[0]);
		std::cout << "Error: Failed to link program!" << std::endl;
		for (unsigned int i = 0; i < infoLog.size(); ++i) {
			std::cout << infoLog[i];
		}
		std::cout << std::endl;
		return false;
	}
	return true;
}

// Shader
Shader::Shader(string _fileName) {
	fileName = _fileName;
	std::map<std::string, ShaderData*>::const_iterator it = sm_resourceMap.find(_fileName);
	if (it != sm_resourceMap.end()) {
		shaderData = it->second;
	} else {
		shaderData = new ShaderData(fileName);
		sm_resourceMap.insert(pair<string, ShaderData*>(fileName, shaderData));
	}
}
Shader::~Shader() {}
void Shader::Enable() const {
	glUseProgram(shaderData->program);
}
void Shader::Disable() const {
	glUseProgram(0);
}
void Shader::SetFloat(string _propertyName, const float& _value) {
	GLuint valueHandle = glGetUniformLocation(shaderData->program, _propertyName.c_str());
	if (valueHandle >= 0) {
		glUniform1f(valueHandle, _value);
	}
}
void Shader::SetInt(string _propertyName, const int& _value) {
	GLuint valueHandle = shaderData->GetLocation(_propertyName);
	if (valueHandle >= 0) {
		glUniform1i(valueHandle, _value);
	}
}
void Shader::SetMatrix4(string _propertyName, const int _size, const mat4& _value, bool _transposed) {
	GLuint valueHandle = shaderData->GetLocation(_propertyName);
	if (valueHandle >= 0) {
		glUniformMatrix4fv(valueHandle, _size, _transposed, (float*)&_value);
	}
}
void Shader::SetVector4(string _propertyName, const vec4& _value) {
	GLuint valueHandle = shaderData->GetLocation(_propertyName);
	if (valueHandle >= 0) {
		glUniform4fv(valueHandle, 1, (float*)&_value);
	}
}
void Shader::SetVector3(string _propertyName, const vec3& _value) {
	GLuint valueHandle = shaderData->GetLocation(_propertyName);
	if (valueHandle >= 0) {
		glUniform3fv(valueHandle, 1, (float*)&_value);
	}
}
void Shader::SetVector3(string _propertyName, const Color& _value) {
	GLuint valueHandle = shaderData->GetLocation(_propertyName);
	if (valueHandle >= 0) {
		glUniform3fv(valueHandle, 1, (float*)&vec3(_value.r, _value.g, _value.b));
	}
}
void Shader::SetFloat2(string _propertyName, const vec2& _value) {
	GLuint valueHandle = glGetUniformLocation(shaderData->program, _propertyName.c_str());
	if (valueHandle >= 0) {
		glUniform2f(valueHandle, _value.x, _value.y);
	}
}
void Shader::SetFloat2(string _propertyName, const float& _a, const float& _b) {
	GLuint valueHandle = glGetUniformLocation(shaderData->program, _propertyName.c_str());
	if (valueHandle >= 0) {
		glUniform2f(valueHandle, _a, _b);
	}
}
void Shader::SetFloat3(string _propertyName, const vec3& _value) {
	GLuint valueHandle = glGetUniformLocation(shaderData->program, _propertyName.c_str());
	if (valueHandle >= 0) {
		glUniform3f(valueHandle, _value.x, _value.y, _value.z);
	}
}
void Shader::SetFloat3(string _propertyName, const float& _a, const float& _b, const float& _c) {
	GLuint valueHandle = glGetUniformLocation(shaderData->program, _propertyName.c_str());
	if (valueHandle >= 0) {
		glUniform3f(valueHandle, _a, _b, _c);
	}
}
void Shader::SetDirectionalLights(const string& _uniformName, const vector<DirectionalLight*>& _dirLights) {
	for (unsigned int i = 0; i < _dirLights.size(); ++i) {
		string index = to_string(i);
		//Base Light
		SetFloat3(_uniformName + "[" + index + "].base.ambient", _dirLights[i]->ambient);
		SetFloat3(_uniformName + "[" + index + "].base.diffuse", _dirLights[i]->diffuse);
		SetFloat3(_uniformName + "[" + index + "].base.specular", _dirLights[i]->specular);
		//Direction Light
		SetFloat3(_uniformName + "[" + index + "].direction", _dirLights[i]->direction);
	}
}
void Shader::SetPointLights(const string& _uniformName, const vector<PointLight*>& _pointLights) {
	for (unsigned int i = 0; i < _pointLights.size(); ++i) {
		string index = to_string(i);
		//Base Light
		SetFloat3(_uniformName + "[" + index + "].base.ambient", _pointLights[i]->ambient);
		SetFloat3(_uniformName + "[" + index + "].base.diffuse", _pointLights[i]->diffuse);
		SetFloat3(_uniformName + "[" + index + "].base.specular", _pointLights[i]->specular);
		//Point Light
		SetFloat(_uniformName + "[" + index + "].atten.constant", _pointLights[i]->attenuation.constant);
		SetFloat(_uniformName + "[" + index + "].atten.linear", _pointLights[i]->attenuation.linear);
		SetFloat(_uniformName + "[" + index + "].atten.quadratic", _pointLights[i]->attenuation.quadratic);
		SetFloat3(_uniformName + "[" + index + "].position", _pointLights[i]->transform->position);
	}
}

// Static
void Shader::Shutdown() {
	for (auto resource : sm_resourceMap) {
		delete resource.second;
	}
	sm_resourceMap.clear();
}
static void CheckShaderError(int _shader, int _flag, bool _isProgram, const string& _errorMessage) {
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (_isProgram) {
		glGetProgramiv(_shader, _flag, &success);
	} else {
		glGetShaderiv(_shader, _flag, &success);
	}

	if (!success) {
		if (_isProgram) {
			glGetProgramInfoLog(_shader, sizeof(error), NULL, error);
		} else {
			glGetShaderInfoLog(_shader, sizeof(error), NULL, error);
		}
		Debug::LogError("Shader error. Line: " + to_string(__LINE__) + " Error: " + _errorMessage.c_str());
	}
}
vector<string> Split(const string& s, char delim) {
	vector<string> elems;

	const char* cstr = s.c_str();
	unsigned int strLength = (unsigned int)s.length();
	unsigned int start = 0;
	unsigned int end = 0;

	while (end <= strLength) {
		while (end <= strLength) {
			if (cstr[end] == delim) {
				break;
			}
			end++;
		}
		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}
	return elems;
}
static string LoadShader(const string& _fileName) {
	std::ifstream file;
	file.open(("./shaders/" + _fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);

			if (line.find("#include") == std::string::npos) {
				output.append(line + "\n");
			} else {
				std::string includeFileName = Split(line, ' ')[1];
				includeFileName = includeFileName.substr(1, includeFileName.length() - 2);

				std::string toAppend = LoadShader(includeFileName);
				output.append(toAppend + "\n");
			}
		}
	} else {
		std::cerr << "Unable to load shader: " << _fileName << std::endl;
		Debug::LogError("Shader error. Line '" + to_string(__LINE__) + " Error: Unable to load shader file '" + _fileName);
	}

	return output;
};
static vector<TypedData> FindUniformStructComponents(const string& _openingBraceToClosingBrace) {
	static const char charsToIgnore[] = { ' ', '\n', '\t', '{' };
	static const size_t UNSIGNED_NEG_ONE = (size_t)-1;

	vector<TypedData> result;
	vector<string> structLines = Split(_openingBraceToClosingBrace, ';');

	for (unsigned int i = 0; i < structLines.size(); i++) {
		size_t nameBegin = UNSIGNED_NEG_ONE;
		size_t nameEnd = UNSIGNED_NEG_ONE;

		for (unsigned int j = 0; j < structLines[i].length(); j++) {
			bool isIgnoreableCharacter = false;

			for (unsigned int k = 0; k < sizeof(charsToIgnore) / sizeof(char); k++) {
				if (structLines[i][j] == charsToIgnore[k]) {
					isIgnoreableCharacter = true;
					break;
				}
			}

			if (nameBegin == UNSIGNED_NEG_ONE && isIgnoreableCharacter == false) {
				nameBegin = j;
			} else if (nameBegin != UNSIGNED_NEG_ONE && isIgnoreableCharacter) {
				nameEnd = j;
				break;
			}
		}

		if (nameBegin == UNSIGNED_NEG_ONE || nameEnd == UNSIGNED_NEG_ONE) {
			continue;
		}

		TypedData newData(
			structLines[i].substr(nameEnd + 1),
			structLines[i].substr(nameBegin, nameEnd - nameBegin));

		result.push_back(newData);
	}

	return result;
}
static string FindUniformStructName(const string& structStartToOpeningBrace) {
	return Split(Split(structStartToOpeningBrace, ' ')[0], '\n')[0];
}
static vector<UniformData> FindUniformStructs(const string& shaderText) {
	static const string STRUCT_KEY = "struct";
	vector<UniformData> result;

	size_t structLocation = shaderText.find(STRUCT_KEY);
	while (structLocation != string::npos) {
		structLocation += STRUCT_KEY.length() + 1; //Ignore the struct keyword and space

		size_t braceOpening = shaderText.find("{", structLocation);
		size_t braceClosing = shaderText.find("}", braceOpening);

		UniformData newStruct(
			FindUniformStructName(shaderText.substr(structLocation, braceOpening - structLocation)),
			FindUniformStructComponents(shaderText.substr(braceOpening, braceClosing - braceOpening)));

		result.push_back(newStruct);
		structLocation = shaderText.find(STRUCT_KEY, structLocation);
	}

	return result;
}
