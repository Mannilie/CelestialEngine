#version 410 

in vec2 _FragTexCoord;

out vec4 _FragColor;

uniform sampler2D _MainTex;

void main()
{
	_FragColor = texture(_MainTex, _FragTexCoord);
}