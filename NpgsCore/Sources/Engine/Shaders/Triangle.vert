#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 Position;
//layout(location = 1) in vec3 Color;
//
//layout(location = 0) out vec3 ColorFromVert;

vec2 Positions[3] = {
	vec2(-0.5,  0.5),
	vec2( 0.5,  0.5),
	vec2( 0.0, -0.5)
};
							 
void main()
{
//	ColorFromVert = Color;
	gl_Position = vec4(Positions[gl_VertexIndex], 0.0, 1.0);
}
