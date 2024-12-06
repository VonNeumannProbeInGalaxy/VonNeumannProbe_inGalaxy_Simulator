#version 460 core

layout(location = 0) out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D iFace;
uniform sampler2D iNpgs;
uniform vec3 iLightPos;
uniform vec3 iLightColor;
uniform vec3 iObjectColor;

void main()
{
#ifdef LAMP_CUBE
	FragColor = vec4(1.0);
	return;
#endif
	vec3 SurfaceNormal = normalize(Normal);
	vec3 LightDir = normalize(iLightPos - FragPos);

	float Diffuse = max(dot(SurfaceNormal, LightDir), 0.0);
	vec3  DiffuseColor = Diffuse * iLightColor;

	vec4 Color = vec4(DiffuseColor * iObjectColor, 1.0);
	FragColor = Color * mix(texture(iNpgs, TexCoord), texture(iFace, TexCoord), 0.3);
}