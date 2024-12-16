#version 460 core

layout(location = 0) in vec3 InputPosition;
layout(location = 1) in vec3 InputNormal;
layout(location = 2) in vec2 InputTexCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4x4 iModel;
uniform mat4x4 iView;
uniform mat4x4 iProjection;
uniform mat3x3 iNormalMatrix;

void main()
{
	TexCoord = InputTexCoord;
	Normal   = normalize(iNormalMatrix * InputNormal);
	FragPos  = vec3(iModel * vec4(InputPosition, 1.0));
	gl_Position = iProjection * iView * vec4(FragPos, 1.0);
}
