#version 460 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D iTexColorBuffer;
uniform sampler2D iLayerBuffer;

void main()
{
	vec4 Result = mix(texture(iTexColorBuffer, TexCoord), texture(iLayerBuffer, TexCoord), 0.2);
	FragColor   = Result;
}
