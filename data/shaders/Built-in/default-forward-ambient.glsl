//Vertex Shader
#if defined(VS_BUILD)
layout(location = 0) in vec3 _Position;
layout(location = 1) in vec2 _TexCoord;
layout(location = 2) in vec3 _Normal;
layout(location = 3) in vec3 _Tangent;

out vec3 _FragPosition;
out vec2 _FragTexCoord;
out vec3 _FragNormal;
out vec3 _FragTangent;

out vec3 _FragWorldPos;
out mat3 _FragTBNMatrix;


uniform mat4 T_model;
uniform mat4 T_MVP;

void main()
{
	_FragPosition = _Position;
	_FragTexCoord = _TexCoord;
	_FragNormal = _Normal;
	_FragTangent = _Tangent;

	_FragWorldPos = (T_model * vec4(_Position, 1.0)).xyz;

	vec3 n = normalize((T_model * vec4(_Normal, 0.0)).xyz);
	vec3 t = normalize((T_model * vec4(_Tangent, 0.0)).xyz);
	t = normalize(t - dot(t, n) * n);

	vec3 biTangent = cross(t, n);
	
	_FragTBNMatrix = mat3(t, biTangent, n);

	gl_Position = T_MVP * vec4(_Position, 1.0);
}

//Fragment Shader
#elif defined(FS_BUILD)
in vec3 _FragPosition;
in vec2 _FragTexCoord;
in vec3 _FragNormal;
in vec3 _FragTangent;

out vec4 _FragColor;

out vec3 _FragWorldPos;
out mat3 _FragTBNMatrix;

uniform vec3 R_ambient;
uniform vec3 C_eyePos;

uniform sampler2D diffuse;
uniform sampler2D dispMap;

uniform float dispMapScale;
uniform float dispMapBias;

void main()
{
	vec3 dirToEye = normalize(C_eyePos - _FragWorldPos);
	
	vec2 texCoords = _FragTexCoord.xy + (dirToEye * _FragTBNMatrix).xy * (texture2D(dispMap, _FragTexCoord.xy).r * dispMapScale + dispMapBias);

	_FragColor = texture2D(diffuse, texCoords) * vec4(R_ambient, 1);
}

#endif
