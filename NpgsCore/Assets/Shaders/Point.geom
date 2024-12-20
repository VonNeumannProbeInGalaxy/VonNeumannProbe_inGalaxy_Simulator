#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 5) out;

layout(location = 0) in  vec3 ColorPassFromVert[];
layout(location = 0) out vec3 ColorPassFromGeom;

void BuildHouse(vec4 Position)
{
	gl_Position = Position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = Position + vec4( 0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = Position + vec4(-0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = Position + vec4( 0.2,  0.2, 0.0, 0.0);
	EmitVertex();
	gl_Position = Position + vec4( 0.0,  0.4, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	ColorPassFromGeom = ColorPassFromVert[0];
	BuildHouse(gl_in[0].gl_Position);
}
