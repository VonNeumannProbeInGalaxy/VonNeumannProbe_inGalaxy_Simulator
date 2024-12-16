#version 460 core

layout(location = 0) out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D iTex;

void main()
{
#ifdef BORDER
	FragColor = vec4(0.04, 0.28, 0.26, 1.0);
	return;
#endif
	vec4 Color = texture(iTex, TexCoord);
	FragColor = Color;
}