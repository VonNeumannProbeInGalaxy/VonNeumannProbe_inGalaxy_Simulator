#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D iTex;

void main()
{
	FragColor = texture(iTex, TexCoord);
}