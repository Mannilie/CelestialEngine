//Vertex Shader
#if defined(VS_BUILD)
layout(location = 0) in vec3 _Vertex;
layout(location = 1) in vec2 _TexCoord;
layout(location = 2) in vec3 _Normal;
layout(location = 3) in vec3 _Tangent;

out vec3 _FragVertex;
out vec2 _FragTexCoord;
out vec3 _FragNormal;
out vec3 _FragTangent;

out vec4 _FragColor;

uniform mat4 T_MVP;

void main()
{
	_FragVertex = _Vertex;
	_FragTexCoord = _TexCoord;
	_FragNormal = _Normal;
	_FragTangent = _Tangent;
	gl_Position = T_MVP * vec4(_Vertex, 1.0);
}

//Fragment Shader
#elif defined(FS_BUILD)
in vec3 _FragVertex;
in vec2 _FragTexCoord;
in vec3 _FragNormal;
in vec3 _FragTangent;

out vec4 _FragColor;

uniform sampler2D diffuse;

void main()
{
	_FragColor = texture2D(diffuse, _FragTexCoord);
}

#endif
