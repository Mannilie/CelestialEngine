#version 410

layout(location = 0) in vec4 _Vertex;
layout(location = 1) in vec2 _TexCoord;

out vec2 _FragTexCoord;

uniform mat4 _ModelMatrix;
uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

void main()
{
	_FragTexCoord = _TexCoord;
	gl_Position	= _ProjectionMatrix * _ViewMatrix * _ModelMatrix * _Vertex;
}