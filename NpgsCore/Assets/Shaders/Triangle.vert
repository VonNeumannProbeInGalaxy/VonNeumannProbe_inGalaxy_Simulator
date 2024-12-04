#version 460 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 InTexCoord;

out vec2 TexCoord;

uniform mat4x4 iModel;
uniform mat4x4 iView;
uniform mat4x4 iProjection;

void main()
{
	TexCoord = InTexCoord;
	mat4x4 MvpMatrix = iProjection * iView * iModel;
	gl_Position = MvpMatrix * vec4(Position, 1.0);
}
