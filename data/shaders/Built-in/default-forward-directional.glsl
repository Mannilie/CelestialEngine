
#include "common.glh"
#include "forwardlighting.glh"

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

uniform mat4 T_MVP;

void main()
{
	_FragPosition = _Position;
	_FragTexCoord = _TexCoord;
	_FragNormal = _Normal;
	_FragTangent = _Tangent;

	gl_Position	= T_MVP * vec4(_Position, 1.0);
}

//Fragment Shader
#elif defined(FS_BUILD)

#include "lighting.glh"

in vec3 _FragPosition;
in vec2 _FragTexCoord;
in vec3 _FragNormal;
in vec3 _FragTangent;

out vec4 _FragColor;

uniform vec3 C_eyePos;
uniform vec3 R_ambient;

uniform float specularIntensity;
uniform float specularPower;

uniform sampler2D diffuse;

uniform DirectionalLight R_directionalLight;

vec4 CalcLightingEffect(vec3 normal, vec3 worldPos)
{
	return CalcLight(R_directionalLight.base, -R_directionalLight.direction, normal, worldPos,
	                 specularIntensity, specularPower, C_eyePos);
}

void main()
{
	vec3 N = normalize(_FragNormal.xyz);
	vec3 L = normalize(vec4(R_directionalLight.direction, 1.0f)).xyz;

	vec3 materialDiffuse = texture(diffuse, _FragTexCoord).xyz;

	vec3 ambient = R_ambient;

	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * R_directionalLight.base.color * materialDiffuse;

	vec3 E = normalize(C_eyePos - _FragPosition.xyz);
	vec3 R = reflect(L, N);
	float s = max(0.0, dot(R, E));
	s = pow(s, specularPower);
	
	//vec3 materialSpecular = texture(specularTexture, _fragTexCoord).xyz;

	vec3 specular = vec3(s) * R_directionalLight.base.color;// * materialSpecular;

	_FragColor = vec4(ambient + diffuse, 1);
}

#endif