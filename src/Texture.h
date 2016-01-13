/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Texture.h
@date: 21/07/2015
@author: Emmanuel Vaccaro
@brief: Base class for texture handling.
===============================================*/

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

// Objects
#include "Object.h"

// Utilities
#include "GLFW_Header.h"

// Other
#include <map>
using std::map;

class TextureData {
public:
	TextureData();
	TextureData(GLenum _textureTarget, int _width, int _height, int _numTextures,
		unsigned char** _pixelData, GLfloat* _filters, GLenum* _internalFormat,
		GLenum* _format, bool _clamp, GLenum* _attachments);
	~TextureData();
	void Bind(int _textureIndex) const;
	void BindAsRenderTarget() const;

	int width;
	int height;
private:
	void InitTextures(unsigned char** _pixelData, GLfloat* _filters,
		GLenum* _internalFormat, GLenum* _format, bool _clamp);
	void InitRenderTargets(GLenum* _attachments);

	GLuint m_frameBuffer;
	GLuint m_renderBuffer;
	int m_numTextures;
	GLenum m_textureTarget;
	GLuint* m_textureID;
};

class Texture {
public:
	Texture(int _width = 0, int _height = 0,
		unsigned char* _data = 0,
		string _fileName = "",
		GLenum _textureTarget = GL_TEXTURE_2D,
		GLfloat _filter = GL_LINEAR,
		GLenum _internalFormat = GL_RGBA,
		GLenum _format = GL_RGBA,
		bool _clamp = false,
		GLenum _attachment = GL_NONE);
	Texture(const std::string& _fileName,
		GLenum _textureTarget = GL_TEXTURE_2D,
		GLfloat _filter = GL_LINEAR,
		GLenum _internalFormat = GL_RGBA,
		GLenum _format = GL_RGBA,
		bool _clamp = false,
		GLenum _attachment = GL_NONE);
	~Texture();
	unsigned int GetTextureHardwareID();
	void Bind(unsigned int _unit = 0) const;
	void BindAsRenderTarget() const;
	int GetWidth() const;
	int GetHeight() const;
	static void Shutdown();
	static void RemoveTexture(string _textureName);

	string fileName;
	int anisoLevel; // Note(Manny): Impliment "anisotropic level".
	int channels;
protected:
	unsigned int textureHardwareID;

	static map<string, TextureData*> sm_resourceMap;

	TextureData* m_textureData;
};

#endif // _TEXTURE_H_