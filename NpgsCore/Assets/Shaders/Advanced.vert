#version 460 core

layout(location = 0) in vec3 InputPosition;
layout(location = 1) in vec2 InputTexCoord;

out vec2 TexCoord;

layout(std140, binding = 0) uniform Matrices
{
    mat4 iModel;
    mat4 iView;
    mat4 iProjection;
};

void main()
{
    TexCoord    = InputTexCoord;    
    gl_Position = iProjection * iView * iModel * vec4(InputPosition, 1.0);
}
