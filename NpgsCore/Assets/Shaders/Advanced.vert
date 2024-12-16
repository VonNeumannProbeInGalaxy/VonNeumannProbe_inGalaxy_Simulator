#version 460 core

layout(location = 0) in vec3 InputPos;
layout(location = 1) in vec2 InputTexCoord;

out vec2 TexCoord;

uniform mat4x4 iModel;
uniform mat4x4 iView;
uniform mat4x4 iProjection;

void main()
{
	TexCoord = InputTexCoord;
	gl_Position = iProjection * iView * iModel * vec4(InputPos, 1.0);
}
