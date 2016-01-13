//COMPOSITE - VERTEX
#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

void main()
{
	fragTexCoord = texCoord;
	gl_Position  = position;	
}