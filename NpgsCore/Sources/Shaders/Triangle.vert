#version 460
#pragma shader_stage(vertex)

vec2 Positions[3] =
{
    {  0.0, -0.5 },
    { -0.5,  0.5 },
    {  0.5,  0.5 }
};

void main()
{
    gl_Position = vec4(Positions[gl_VertexIndex], 0.0, 1.0);
}
