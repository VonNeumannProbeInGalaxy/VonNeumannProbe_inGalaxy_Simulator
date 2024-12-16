#version 460 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D iTexColorBuffer;

void main()
{
	vec4 Result = texture(iTexColorBuffer, TexCoord);
	FragColor   = Result;
}
