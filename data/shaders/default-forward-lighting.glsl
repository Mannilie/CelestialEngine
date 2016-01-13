//Vertex Shader
#if defined(VS_BUILD)
layout(location = 0) in vec3 _Vertex;
layout(location = 1) in vec2 _TexCoord;

layout(location = 2) in vec4 _BoneIndices;
layout(location = 3) in vec4 _BoneWeights;

layout(location = 4) in vec3 _Normal;
layout(location = 5) in vec3 _Tangent;

out vec3 _FragVertex;
out vec2 _FragTexCoord;
out vec3 _FragNormal;
out vec3 _FragTangent;

out vec3 _FragBiTangent;

out vec3 _FragPosition;

uniform mat4 T_model;
uniform mat4 C_viewProj;

void main()
{
	_FragVertex = _Vertex;
	_FragTexCoord = _TexCoord;
	_FragNormal = mat3(transpose(inverse(T_model))) * _Normal;
	_FragTangent = _Tangent;
	_FragPosition = vec3(T_model * vec4(_Vertex, 1.0));
	_FragBiTangent = cross(_Normal, _Tangent);

	gl_Position = C_viewProj * T_model * vec4(_Vertex, 1.0);
}

//Fragment Shader
#elif defined(FS_BUILD)

#include "lighting.glh"

#define DIR_LIGHT_MAX 20
#define POINT_LIGHT_MAX 20

in vec3 _FragVertex;
in vec2 _FragTexCoord;
in vec3 _FragNormal;
in vec3 _FragTangent;

in vec3 _FragBiTangent;

in vec3 _FragPosition;

out vec4 _FragColor;

uniform vec3 C_eyePos;
uniform DirLight R_dirLights[DIR_LIGHT_MAX];
uniform PointLight R_pointLights[POINT_LIGHT_MAX];

uniform int R_DIR_LIGHT_COUNT;
uniform int R_POINT_LIGHT_COUNT;

uniform sampler2D M_diffuse;

uniform sampler2D M_normalMap;
uniform float M_specularPower;

uniform sampler2D M_specMap;
uniform sampler2D M_dispMap;

uniform float M_dispMapScale;
uniform float M_dispMapBias;

vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir, vec2 texCoord);
vec3 CalcPointLight(PointLight light, vec3 normalDir, vec3 fragPos, vec3 viewDir, vec2 texCoord);

void main()
{
	mat3 TBN = mat3(normalize(_FragTangent),
					normalize(_FragBiTangent),
					normalize(_FragNormal));

	vec3 normalSample = vec3(texture(M_normalMap, _FragTexCoord));
	vec3 adjustedNormal = normalSample * 2.0 - 1.0;

	vec3 normalDir = normalize(TBN * adjustedNormal);

	//vec3 norm = normalize(_FragNormal);
	vec3 viewDir = normalize(C_eyePos - _FragPosition);
	vec2 texCoord = _FragTexCoord.xy + (viewDir * TBN).xy * (texture2D(M_dispMap, _FragTexCoord.xy).r * M_dispMapScale + M_dispMapBias);
	// == ======================================
    // Lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == ======================================
    // Phase 1: Directional lighting
    vec3 result = vec3(0, 0, 0); //CalcDirLight(R_dirLights[0], normalDir, viewDir, texCoord);;
	for(int i = 0; i < R_DIR_LIGHT_COUNT; i++)
		result += CalcDirLight(R_dirLights[i], normalDir, viewDir, texCoord);
    // Phase 2: Point lights
	for(int i = 0; i < R_POINT_LIGHT_COUNT; i++)
		result += CalcPointLight(R_pointLights[i], normalDir, _FragPosition, viewDir, texCoord);    
    // Phase 3: Spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

	_FragColor = vec4(result, 1.0);
} 

//Calculates the color when using a directinal light.
vec3 CalcDirLight(DirLight light, vec3 normalDir, vec3 viewDir, vec2 texCoord)
{
	vec3 lightDir = normalize(light.direction);
	//Diffuse shading
	float diff = max(dot(-lightDir, normalDir), 0.0);
	//Specular shading
	vec3 reflectDir = reflect(lightDir, normalDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), M_specularPower);
	//Combine results
	vec3 ambientColor = light.base.ambient * vec3(texture(M_diffuse, texCoord));
	vec3 diffuseColor = light.base.diffuse * diff * vec3(texture(M_diffuse, texCoord));
	vec3 specularColor = light.base.specular * spec * vec3(texture(M_specMap, texCoord));
	return (ambientColor + diffuseColor + specularColor);
}

//Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normalDir, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	vec3 lightDir = normalize(light.position - fragPos);
	//Diffuse shading
	float diff = max(dot(normalDir, lightDir), 0.0);
	//Specular shading

	vec3 reflectDir = reflect(-lightDir, normalDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), M_specularPower);

	//Attenuation
	float dist = length(light.position - fragPos);
	float attenuation = 1.0f / (light.atten.constant + light.atten.linear * dist + light.atten.quadratic * (dist * dist));
	
	//Combine results
	vec3 ambientColor = light.base.ambient * vec3(texture(M_diffuse, texCoord));
	vec3 diffuseColor = light.base.diffuse * diff * vec3(texture(M_diffuse, texCoord));
	vec3 specularColor = light.base.specular * spec * vec3(texture(M_specMap, texCoord));
	ambientColor *= attenuation;
	diffuseColor *= attenuation;
	specularColor *= attenuation;
	return (ambientColor + diffuseColor + specularColor);
}

#endif
