#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D iFace;
uniform sampler2D iNpgs;

void main()
{
	FragColor = mix(texture(iNpgs, TexCoord), texture(iFace, TexCoord), 0.3);
}