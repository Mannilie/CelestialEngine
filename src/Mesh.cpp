#include "Mesh.h"

#include "RenderingEngine.h"
#include "imgui.h"
#include "Gizmos.h"
#include "Time.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

// Static resource map
map<string, IndexedModel*> Mesh::sm_resourceMap;

// MeshData 

MeshData::~MeshData() {
	glDeleteVertexArrays(1, &glData.VAO);
	glDeleteBuffers(1, &glData.VBO);
	glDeleteBuffers(1, &glData.IBO);
}

bool MeshData::IsValid() const {
	return positions.size() == texCoords.size() &&
		texCoords.size() == boneIndices.size() &&
		boneIndices.size() == boneWeights.size() &&
		boneWeights.size() == normals.size() &&
		normals.size() == tangents.size();
}

void MeshData::CalculateNormals() {
	normals.clear();
	normals.reserve(positions.size());

	for (unsigned int i = 0; i < positions.size(); ++i) {
		normals.push_back(vec3(0.0f));
	}

	for (unsigned int i = 0; i < normals.size(); i += 3) {
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		vec3 v1 = positions[i1] - positions[i0];
		vec3 v2 = positions[i2] - positions[i0];

		vec3 normal = glm::normalize(glm::cross(v1, v2));

		normals[i0] += normal;
		normals[i1] += normal;
		normals[i2] += normal;
	}

	for (unsigned int i = 0; i < normals.size(); ++i) {
		normals[i] = glm::normalize(normals[i]);
	}
}

void MeshData::CalculateTangents() {
	tangents.clear();
	tangents.reserve(positions.size());

	for (unsigned int i = 0; i < positions.size(); ++i) {
		tangents.push_back(vec3(0.0f));
	}

	for (unsigned int i = 0; i < indices.size(); i += 3) {
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		vec3 edge1 = positions[i1] - positions[i0];
		vec3 edge2 = positions[i2] - positions[i0];

		float deltaU1 = texCoords[i1].x - texCoords[i0].x;
		float deltaU2 = texCoords[i2].x - texCoords[i0].x;
		float deltaV1 = texCoords[i1].y - texCoords[i0].y;
		float deltaV2 = texCoords[i2].y - texCoords[i0].y;

		float denom = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		float f = denom == 0.0f ? 0.0f : 1.0f / denom;

		vec3 tangent = vec3(0);

		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		tangents[i0] += tangent;
		tangents[i1] += tangent;
		tangents[i2] += tangent;
	}

	for (unsigned int i = 0; i < tangents.size(); ++i) {
		tangents[i] = glm::normalize(tangents[i]);
	}
}

MeshData MeshData::Finalize() {
	// Check if mesh data is valid
	if (IsValid()) {
		return *this;
	}

	// TexCoords
	if (texCoords.size() == 0) {
		for (unsigned int i = texCoords.size(); i < positions.size(); ++i) {
			texCoords.push_back(vec2(0.0f));
		}
	}

	// Bone Indices
	if (boneIndices.size() == 0) {
		for (unsigned int i = boneIndices.size(); i < texCoords.size(); ++i) {
			boneIndices.push_back(vec4(1.0f));
		}
	}

	// Bone Weights
	if (boneWeights.size() == 0) {
		for (unsigned int i = boneWeights.size(); i < boneIndices.size(); ++i) {
			boneWeights.push_back(vec4(1.0f));
		}
	}

	// Normals
	if (normals.size() == 0) {
		CalculateNormals();
	}

	// Tangents
	if (tangents.size() == 0) {
		CalculateTangents();
	}

	return *this;
}

void MeshData::AddVertex(const vec3& _vert) {
	positions.push_back(_vert);
}

void MeshData::AddTexCoord(const vec2& _texCoord) {
	texCoords.push_back(_texCoord);
}

void MeshData::AddBoneIndices(const vec4& _boneIndices) {
	boneIndices.push_back(_boneIndices);
}

void MeshData::AddBoneWeights(const vec4& _boneWeights) {
	boneWeights.push_back(_boneWeights);
}

void MeshData::AddNormal(const vec3& _normal) {
	normals.push_back(_normal);
}

void MeshData::AddTangent(const vec3& _tangent) {
	tangents.push_back(_tangent);
}

void MeshData::AddIndices(const GLuint& _indices) {
	indices.push_back(_indices);
}

void MeshData::AddFace(GLuint _vertIndex0, GLuint _vertIndex1, GLuint _vertIndex2) {
	indices.push_back(_vertIndex0);
	indices.push_back(_vertIndex1);
	indices.push_back(_vertIndex2);
}

// Indexed Model
void IndexedModel::Init() {
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		MeshData& currMesh = meshes[i];
		if (!currMesh.IsValid()) {
			std::cout << "Error: Invalid mesh! Must have same number of positions, "
				<< "texCoords, normals, and tangents! (Maybe you forgot to Finalize()"
				<< "your IndexedModel?)" << std::endl;
			return;
		}

		glGenVertexArrays(1, &currMesh.glData.VAO);
		glGenBuffers(1, &currMesh.glData.VBO);
		glGenBuffers(1, &currMesh.glData.IBO);
		
		vector<float> vertexData;

		// Positions
		for (unsigned int i = 0; i < currMesh.positions.size(); ++i) {
			vertexData.push_back(currMesh.positions[i].x);
			vertexData.push_back(currMesh.positions[i].y);
			vertexData.push_back(currMesh.positions[i].z);
		}

		// TexCoords
		for (unsigned int i = 0; i < currMesh.texCoords.size(); ++i) {
			vertexData.push_back(currMesh.texCoords[i].x);
			vertexData.push_back(currMesh.texCoords[i].y);
		}
		
		// Bone Indices
		for (unsigned int i = 0; i < currMesh.boneIndices.size(); ++i) {
			vertexData.push_back(currMesh.boneIndices[i].x);
			vertexData.push_back(currMesh.boneIndices[i].y);
			vertexData.push_back(currMesh.boneIndices[i].z);
			vertexData.push_back(currMesh.boneIndices[i].w);
		}

		// Bone Weights
		for (unsigned int i = 0; i < currMesh.boneWeights.size(); ++i) {
			vertexData.push_back(currMesh.boneWeights[i].x);
			vertexData.push_back(currMesh.boneWeights[i].y);
			vertexData.push_back(currMesh.boneWeights[i].z);
			vertexData.push_back(currMesh.boneWeights[i].w);
		}

		// Normals
		for (unsigned int i = 0; i < currMesh.normals.size(); ++i) {
			vertexData.push_back(currMesh.normals[i].x);
			vertexData.push_back(currMesh.normals[i].y);
			vertexData.push_back(currMesh.normals[i].z);
		}

		// Tangents
		for (unsigned int i = 0; i < currMesh.tangents.size(); ++i) {
			vertexData.push_back(currMesh.tangents[i].x);
			vertexData.push_back(currMesh.tangents[i].y);
			vertexData.push_back(currMesh.tangents[i].z);
		}

		currMesh.glData.indexCount = currMesh.indices.size();

		glBindVertexArray(currMesh.glData.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, currMesh.glData.VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currMesh.glData.IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, currMesh.indices.size() * sizeof(GLuint), &currMesh.indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Positions
		glEnableVertexAttribArray(1); // TexCoords
		
		glEnableVertexAttribArray(2); // Bone Indices
		glEnableVertexAttribArray(3); // Bone Weights

		glEnableVertexAttribArray(4); // Normals
		glEnableVertexAttribArray(5); // Tangents

		unsigned int positionsOffset = sizeof(vec3) * currMesh.positions.size();
		unsigned int texCoordsOffset = sizeof(vec2) * currMesh.tangents.size();
		
		unsigned int boneIndicesOffset = sizeof(vec4) * currMesh.boneIndices.size();
		unsigned int boneWeightsOffset = sizeof(vec4) * currMesh.boneWeights.size();

		unsigned int normalsOffset = sizeof(vec3) * currMesh.normals.size();
		unsigned int tangentsOffset = sizeof(vec3) * currMesh.tangents.size();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(positionsOffset));

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(positionsOffset + texCoordsOffset));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)(positionsOffset + texCoordsOffset + boneIndicesOffset));

		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positionsOffset + texCoordsOffset + boneIndicesOffset + boneWeightsOffset));
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positionsOffset + texCoordsOffset + boneIndicesOffset + boneWeightsOffset + normalsOffset));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void IndexedModel::Draw(RenderingEngine& _renderer) {
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		glBindVertexArray(meshes[i].glData.VAO);
		glDrawElements(GL_TRIANGLES, meshes[i].glData.indexCount, GL_UNSIGNED_INT, 0);
	}
}

void IndexedModel::Finalize() {
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		meshes[i].Finalize();
	}
}


void IndexedModel::AddSkeleton(FBXSkeleton* _skeleton) {
	skeletons.push_back(_skeleton);
}

void IndexedModel::AddAnimation(FBXAnimation* _animation) {
	animations.push_back(_animation);
}

// Mesh
Mesh::Mesh(const string& _fileName, 
	bool _useDefaultDir) : 
	fileName(_fileName), 
	shader(Shader("default-forward-lighting")) {
	Init(_fileName, _useDefaultDir);
}

Mesh::Mesh(const string& _fileName, 
	const Shader& _shader, 
	bool _useDefaultDir) : 
	fileName(_fileName), 
	shader(_shader) {
	Init(_fileName, _useDefaultDir);
}

Mesh::~Mesh(){}

void Mesh::Init(const string& _fileName, bool _useDefaultDir) {
	map<string, IndexedModel*>::const_iterator it = sm_resourceMap.find(_fileName);
	if (it != sm_resourceMap.end()) {
		model = it->second;
	} else {
		model = new IndexedModel();

		string fullDir;
		if (_useDefaultDir) {
			fullDir = "models/" + _fileName;
		} else {
			fullDir = _fileName;
		}

		string fileExtension = _fileName.substr(_fileName.find_last_of("."), _fileName.length());

		if (fileExtension == ".obj" || fileExtension == ".OBJ") {
			LoadOBJFile(fullDir);
		} else if (fileExtension == ".fbx" || fileExtension == ".FBX") {
			LoadFBXFile(fullDir);
		}

		CalculateMeshBounds();
		sm_resourceMap.insert(pair<string, IndexedModel*>(_fileName, model));

		model->Finalize();
		model->Init();
	}
}

void Mesh::Draw(RenderingEngine& _renderer) {
	model->Draw(_renderer);
}

Bounds Mesh::CalculateMeshBounds() {
	if (model->meshes.size() > 0) {
		vec3 min = vec3(FLT_MAX);
		vec3 max = vec3(FLT_MIN);
		vector<vec3>& positions = model->meshes[0].positions; //Note(Manny): Loop through all the meshes and calculate bounds for each
		for (unsigned int i = 0; i < positions.size(); ++i) {
			if (positions[i].x < min.x) min.x = positions[i].x;
			if (positions[i].y < min.y) min.y = positions[i].y;
			if (positions[i].z < min.z) min.z = positions[i].z;

			if (positions[i].x > max.x) max.x = positions[i].x;
			if (positions[i].y > max.y) max.y = positions[i].y;
			if (positions[i].z > max.z) max.z = positions[i].z;
		}
		bounds.SetMinMax(min, max);
	}
	return bounds;
}

void Mesh::Inspector() {
	ImGui::Text(("Mesh: " + fileName).c_str());
}

void Mesh::LoadOBJFile(const string& _fileName) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(_fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace);

	if (!scene) {
		std::cout << "Mesh load failed!: " << _fileName << std::endl;
		return;
	}

	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		const aiMesh* mesh = scene->mMeshes[meshIndex];
		const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
		
		MeshData newMeshData;

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			const aiVector3D pos = mesh->mVertices[i];
			const aiVector3D normal = mesh->mNormals[i];

			// Note(Manny): Place Bone Indices and Bone Weights here
			
			const aiVector3D texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiZeroVector;
			const aiVector3D tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : aiZeroVector;

			newMeshData.AddVertex(vec3(pos.x, pos.y, pos.z));
			newMeshData.AddNormal(vec3(normal.x, normal.y, normal.z));
			newMeshData.AddTexCoord(vec2(texCoord.x, texCoord.y));
			newMeshData.AddTangent(vec3(tangent.x, tangent.y, tangent.z));
		}
		
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			newMeshData.AddFace(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
		}

		model->meshes.push_back(newMeshData);
	}
}

void Mesh::LoadFBXFile(const string& _fileName) {
	// Create a new FBX File
	FBXFile* fbxFile = new FBXFile();
	
	// Load the FBX file
	fbxFile->load(_fileName.c_str());

	// Get the Skeletons from file
	unsigned int skeletonCount = fbxFile->getSkeletonCount();
	for (unsigned int skeleIndex = 0; skeleIndex < skeletonCount; ++skeleIndex) {
		FBXSkeleton* currentSkeleton = fbxFile->getSkeletonByIndex(skeleIndex);
		model->AddSkeleton(currentSkeleton);
	}

	// Get the Animations from file
	unsigned int animationCount = fbxFile->getAnimationCount();
	for (unsigned int animIndex = 0; animIndex < animationCount; ++animIndex) {
		FBXAnimation* currentAnimation = fbxFile->getAnimationByIndex(animIndex);
		model->AddAnimation(currentAnimation);
	}

	// Get number of meshes in FBX file
	unsigned int meshCount = fbxFile->getMeshCount();

	// Loop through all meshes in fbx file
	for (unsigned int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
		FBXMeshNode* currentMesh = fbxFile->getMeshByIndex(meshIndex);

		MeshData newMeshData;
		for (unsigned int i = 0; i < currentMesh->m_vertices.size(); ++i) {
			FBXVertex vertex = currentMesh->m_vertices[i];
			newMeshData.AddVertex(vec3(vertex.position.x, vertex.position.y, vertex.position.z));
			newMeshData.AddNormal(vec3(vertex.normal.x, vertex.normal.y, vertex.normal.z));
			newMeshData.AddBoneIndices(vec4(vertex.indices.x, vertex.indices.y, vertex.indices.z, vertex.indices.w));
			newMeshData.AddBoneWeights(vec4(vertex.weights.x, vertex.weights.y, vertex.weights.z, vertex.weights.w));
			newMeshData.AddTexCoord(vec2(vertex.texCoord1.x, vertex.texCoord1.y));
			newMeshData.AddTangent(vec3(vertex.tangent.x, vertex.tangent.y, vertex.tangent.z));
		}

		for (unsigned int i = 0; i < currentMesh->m_indices.size(); ++i) {
			newMeshData.AddIndices(currentMesh->m_indices[i]); 
		}

		Texture diffuse = CreateFBXTexture(currentMesh->m_material->textures[FBXMaterial::DiffuseTexture]);
		Texture normal = CreateFBXTexture(currentMesh->m_material->textures[FBXMaterial::NormalTexture]);
		Texture specular = CreateFBXTexture(currentMesh->m_material->textures[FBXMaterial::SpecularTexture]);

		if (normal.fileName == "") {
			normal = Texture("default_normal.png");
		}

		if (specular.fileName == "") {
			specular = Texture("default_specular.png");
		}

		Material newMeshMaterial(currentMesh->m_material->name, diffuse, specular, 0, 20.0f, normal);
		model->materials.push_back(newMeshMaterial);
		model->meshes.push_back(newMeshData);
	}
}

Texture Mesh::CreateFBXTexture(FBXTexture* _fbxTexture) {
	if (_fbxTexture == nullptr) {
		return Texture();
	}

	GLuint format = 0;
	switch (_fbxTexture->format) {
		case 1:	format = GL_RED; break;
		case 2:	format = GL_RG; break;
		case 3:	format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		case 5: format = GL_BGRA; break;
		case 6: format = GL_RED_INTEGER; break;
		case 7: format = GL_RG_INTEGER; break;
		case 8: format = GL_RGB_INTEGER; break;
		case 9: format = GL_BGR_INTEGER; break;
		case 10: format = GL_RGBA_INTEGER; break;
		case 11: format = GL_BGRA_INTEGER; break;
		case 12: format = GL_STENCIL_INDEX; break;
		case 13: format = GL_DEPTH_COMPONENT; break;
		case 14: format = GL_DEPTH_STENCIL; break;
	}

	return Texture(_fbxTexture->width,
		_fbxTexture->height,
		_fbxTexture->data, 
		_fbxTexture->name, 
		GL_TEXTURE_2D,
		GL_NEAREST, 
		format, 
		format);
}

void Mesh::UpdateAllBones() {
	for (unsigned int skeleIndex = 0; skeleIndex < model->skeletons.size(); ++skeleIndex) {
		FBXSkeleton& skele = *model->skeletons[skeleIndex];

		// Loop through all bones
		for (unsigned int boneIndex = 0; boneIndex < skele.m_boneCount; ++boneIndex) {
			// Generate their global transforms
			int parentIndex = skele.m_parentIndex[boneIndex];
			mat4& bone = skele.m_bones[boneIndex];
			FBXNode* node = skele.m_nodes[boneIndex];
			if (parentIndex == -1) {
				bone = node->m_localTransform;
			} else {
				mat4& parentBone = skele.m_bones[parentIndex];
				bone = parentBone * node->m_localTransform;
			}
		}

		for (unsigned int boneIndex = 0; boneIndex < skele.m_boneCount; ++boneIndex) {
			// Multiply the global transform by the inverse bind pose
			mat4& bone = skele.m_bones[boneIndex];
			mat4& bindPose = skele.m_bindPoses[boneIndex];
			bone = bone * bindPose;
		}
	}
}

void Mesh::DrawGizmosBones() {
	for (unsigned int skeleIndex = 0; skeleIndex < model->skeletons.size(); ++skeleIndex) {
		FBXSkeleton& skele = *model->skeletons[skeleIndex];

		// Loop through all bones
		for (unsigned int boneIndex = 0; boneIndex < skele.m_boneCount; ++boneIndex) {
			FBXNode* node = skele.m_nodes[boneIndex];
			FBXNode* parentNode = node->m_parent;

			node->updateGlobalTransform();

			mat4& nodeGlobal = node->m_globalTransform;
			vec3 nodePos = nodeGlobal[3].xyz;

			Gizmos::AddAABBFilled(nodePos, vec3(3.0f), vec4(1, 0, 0, 1), &nodeGlobal);

			if (parentNode != nullptr) {
				vec3 parentPos = parentNode->m_globalTransform[3].xyz;
				Gizmos::AddLine(nodePos, parentPos, vec4(0, 1, 0, 1));
			}
		}
	}
}

void Mesh::Shutdown() {
	// Delete all resources
	for (auto resource : sm_resourceMap) {
		delete resource.second;
	}
	// Clear the map
	sm_resourceMap.clear();
}