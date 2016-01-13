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
out vec3 _FragPosition;
out vec2 _FragTexCoord;
out vec3 _FragNormal;
out vec3 _FragTangent;

out vec4 _FragColor;


uniform vec3 R_ambient;
uniform vec3 C_eyePos;

uniform vec3 C_lightDir;
uniform vec3 lightColor;

uniform float specularPower;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

void main()
{
	vec3 N = normalize(_FragNormal.xyz);
	vec3 L = normalize(_FragModelView * vec4(lightDir, 1.0f)).xyz;

	vec3 materialDiffuse = texture(diffuseTexture, _fragTexCoord).xyz;

	vec3 ambient = _Color * ambientLight;

	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * lightColor * _Color;

	vec3 E = normalize(eyePos - _fragVertex.xyz);
	vec3 R = reflect(L, N);
	float s = max(0.0, dot(R, E));
	s = pow(s, specularPower);
	
	vec3 materialSpecular = texture(specularTexture, _fragTexCoord).xyz;

	vec3 specular = vec3(s) * lightColor;// * materialSpecular;

	_fragColor = vec4(ambient + diffuse + specular, 1);
}

#endif