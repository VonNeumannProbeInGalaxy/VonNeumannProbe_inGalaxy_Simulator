#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform float     iTime;
uniform sampler2D iFace;
uniform sampler2D iNpgs;

void main()
{
	FragColor = mix(texture(iNpgs, TexCoord), texture(iFace, TexCoord), sin(iTime));
}