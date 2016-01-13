#include "Texture.h"

// Utilities
#include "GLM_Header.h"
#include "stb_image.h"

// Other
#include <iostream>

map<string, TextureData*> Texture::sm_resourceMap;

// TextureData
TextureData::TextureData(GLenum _textureTarget, int _width, int _height, int _numTextures,
	unsigned char** _pixelData, GLfloat* _filters, GLenum* _internalFormat,
	GLenum* _format, bool _clamp, GLenum* _attachments) {
	m_textureID = new GLuint[_numTextures];
	m_textureTarget = _textureTarget;
	m_numTextures = _numTextures;

	width = _width;
	height = _height;

	m_frameBuffer = 0;
	m_renderBuffer = 0;

	InitTextures(_pixelData, _filters, _internalFormat, _format, _clamp);
	InitRenderTargets(_attachments);
}
TextureData::~TextureData() {
	if (*m_textureID) { 
		glDeleteTextures(m_numTextures, m_textureID); 
	}
	if (m_frameBuffer) {
		glDeleteFramebuffers(1, &m_frameBuffer);
	}
	if (m_renderBuffer) {
		glDeleteRenderbuffers(1, &m_renderBuffer);
	}
	if (m_textureID) {
		delete[] m_textureID;
	}
}
void TextureData::InitTextures(unsigned char** _pixelData, GLfloat* _filters,
	GLenum* _internalFormat, GLenum* _format, bool _clamp) {
	glGenTextures(m_numTextures, m_textureID);
	for (int i = 0; i < m_numTextures; i++) {
		glBindTexture(m_textureTarget, m_textureID[i]);

		glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, _filters[i]);
		glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, _filters[i]);

		if (_clamp) {
			glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(m_textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glTexImage2D(m_textureTarget, 0, _internalFormat[i], width, height, 0, _format[i], GL_UNSIGNED_BYTE, _pixelData[i]);

		if (_filters[i] == GL_NEAREST_MIPMAP_NEAREST ||
			_filters[i] == GL_NEAREST_MIPMAP_LINEAR ||
			_filters[i] == GL_LINEAR_MIPMAP_NEAREST ||
			_filters[i] == GL_LINEAR_MIPMAP_LINEAR) {
			glGenerateMipmap(m_textureTarget);
			GLfloat maxAnisotropy;
			glGetFloatv(GL_TEXTURE_MAG_FILTER, &maxAnisotropy);
			glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, glm::clamp(0.0f, 8.0f, maxAnisotropy));
		} else {
			glTexParameteri(m_textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(m_textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		}
	}
}
void TextureData::InitRenderTargets(GLenum* _attachments) {
	if (_attachments == nullptr) {
		return;
	}
	GLenum drawBuffers[32];
	bool hasDepth = false;
	for (int i = 0; i < m_numTextures; ++i) {
		if (_attachments[i] == GL_DEPTH_ATTACHMENT) {
			drawBuffers[i] = GL_NONE;
			hasDepth = true;
		} else {
			drawBuffers[i] = _attachments[i];
		}

		if (_attachments[i] == GL_NONE) {
			continue;
		}

		if (m_frameBuffer == 0) {
			glGenFramebuffers(1, &m_frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, _attachments[i], m_textureTarget, m_textureID[i], 0);
	}

	if (m_frameBuffer == 0) {
		return;
	}

	if (!hasDepth) {
		glGenRenderbuffers(1, &m_renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);
	}

	glDrawBuffers(m_numTextures, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer creation failed!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void TextureData::Bind(int _textureIndex) const {
	glBindTexture(m_textureTarget, m_textureID[_textureIndex]);
}
void TextureData::BindAsRenderTarget() const {
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glViewport(0, 0, width, height);
}

// Texture
Texture::Texture(int _width, int _height,
	unsigned char* _data,
	string _fileName, 
	GLenum _textureTarget,
	GLfloat _filter,
	GLenum _internalFormat,
	GLenum _format,
	bool _clamp,
	GLenum _attachment) {
	fileName = _fileName;
	map<string, TextureData*>::const_iterator it = sm_resourceMap.find(fileName);
	if (it != sm_resourceMap.end()) {
		m_textureData = it->second;
	} else {
		m_textureData = new TextureData(_textureTarget, _width, _height, 1, &_data, &_filter, &_internalFormat, &_format, _clamp, &_attachment);
		sm_resourceMap.insert(std::pair<std::string, TextureData*>(fileName, m_textureData));
	}
}
Texture::Texture(const string& _fileName,
	GLenum _textureTarget,
	GLfloat _filter,
	GLenum _internalFormat,
	GLenum _format,
	bool _clamp,
	GLenum _attachment) {
	fileName = _fileName;
	map<string, TextureData*>::const_iterator it = sm_resourceMap.find(_fileName);
	if (it != sm_resourceMap.end()) {
		m_textureData = it->second;
	} else {
		int x, y, bytesPerPixel;
		unsigned char* data = stbi_load(("./textures/" + _fileName).c_str(), &x, &y, &bytesPerPixel, 4);

		if (data == NULL) {
			std::cerr << "Unable to load texture: " << _fileName << std::endl;
		}

		m_textureData = new TextureData(_textureTarget, x, y, 1, &data, &_filter, &_internalFormat, &_format, _clamp, &_attachment);
		stbi_image_free(data);

		sm_resourceMap.insert(std::pair<std::string, TextureData*>(_fileName, m_textureData));
	}
}
Texture::~Texture() {}
unsigned int Texture::GetTextureHardwareID() {
	return textureHardwareID;
}
void Texture::Bind(unsigned int _unit) const {
	glActiveTexture(GL_TEXTURE0 + _unit);
	m_textureData->Bind(0);
}
void Texture::BindAsRenderTarget() const {
	m_textureData->BindAsRenderTarget();
}
int Texture::GetWidth() const {
	return m_textureData->width;
}
int Texture::GetHeight() const {
	return m_textureData->height;
}
void Texture::Shutdown() {
	for (auto resource : sm_resourceMap) {
		delete resource.second;
	}
	sm_resourceMap.clear();
}
void Texture::RemoveTexture(string _textureName) {
	for (auto resource : sm_resourceMap) {
		if (resource.first == _textureName) {
			delete resource.second;
			sm_resourceMap.erase(resource.first);
			return;
		}
	}
}