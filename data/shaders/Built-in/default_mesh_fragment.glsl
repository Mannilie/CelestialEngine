#version 410 

uniform vec4 _Color;

out vec4 _fragColor;

void main()
{
	_fragColor = _Color;
}