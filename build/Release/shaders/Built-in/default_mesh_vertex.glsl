#version 410

layout(location = 0) in vec4 _Vertex;

uniform mat4 _ModelMatrix;
uniform mat4 _ProjectionMatrix;
uniform mat4 _ViewMatrix;

void main()
{
	gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * _Vertex;
}