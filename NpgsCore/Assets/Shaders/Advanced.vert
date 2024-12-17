#version 460 core
#extension GL_NV_uniform_buffer_std430_layout : enable

layout(location = 0) in vec3 InputPosition;
layout(location = 1) in vec2 InputTexCoord;

out vec2 TexCoord;

// 1. shared layout - 编译器可以自由优化布局
layout(shared, binding = 0) uniform Matrices
{
    mat4 iModel;      // offset = 0  (编译器决定)
    mat4 iView;       // offset = 64 (编译器决定)
    mat4 iProjection; // offset = 128 (编译器决定)
};

// 2. std140 layout - 固定布局规则
layout(std140, binding = 1) uniform Materials 
{
    vec3 baseColor;     // offset = 0,  size = 12, aligned to 16
    float roughness;    // offset = 12, size = 4
    vec2 uvScale;       // offset = 16, size = 8, aligned to 8
    bool[3] flags;      // offset = 24, size = 3*4 = 12, array aligned to 16
    mat3 normalMatrix;  // offset = 48, size = 48 (3 columns, each vec3 aligned to 16)
    vec4 parameters[2]; // offset = 96, size = 32 (array elements aligned to 16)
};

// 3. std430 layout - 紧凑布局
layout(std430, binding = 2) uniform Lights
{
    vec3 lightPositions[4];    // offset = 0,   size = 48  (紧凑数组)
    vec3 lightColors[4];       // offset = 48,  size = 48
    float lightIntensities[4]; // offset = 96,  size = 16
    bool lightEnabled[4];      // offset = 112, size = 16
    mat4x3 lightTransforms[2]; // offset = 128, size = 96  (每个矩阵紧凑存储)
};

struct Light {
    vec3 position;
    float intensity;
    vec4 color;
};

layout(std140, binding = 3) uniform ComplexBlock
{
    float scalar;          // offset = 0,    size = 4,    align = 4
    vec2 vector2;          // offset = 8,    size = 8,    align = 8
    vec3 vector3;          // offset = 16,   size = 12,   align = 16
    vec4 vector4;          // offset = 32,   size = 16,   align = 16
    int intScalar;         // offset = 48,   size = 4,    align = 4
    ivec2 intVector2;      // offset = 56,   size = 8,    align = 8
    ivec3 intVector3;      // offset = 64,   size = 12,   align = 16
    ivec4 intVector4;      // offset = 80,   size = 16,   align = 16
    
    mat2 matrix2;          // offset = 96,   size = 32,   align = 16 (2 vec2s)
    mat3 matrix3;          // offset = 128,  size = 48,   align = 16 (3 vec3s)
    mat4 matrix4;          // offset = 176,  size = 64,   align = 16 (4 vec4s)
    mat2x3 matrix2x3;      // offset = 240,  size = 32,   align = 16 (2 vec3s)
    mat3x2 matrix3x2;      // offset = 272,  size = 48,   align = 16 (3 vec2s)
    
    float floatArray[4];   // offset = 320,  size = 16,   align = 16
    vec2 vec2Array[3];     // offset = 336,  size = 48,   align = 16
    vec3 vec3Array[2];     // offset = 384,  size = 32,   align = 16
    vec4 vec4Array[2];     // offset = 416,  size = 32,   align = 16
    
    Light lights[2];       // offset = 448,  size = 64,   align = 16 (2 * (16+16))
    
    mat2 matrixArray[2];   // offset = 512,  size = 64,   align = 16
    
    bool flag;             // offset = 576,  size = 4,    align = 4
    bvec2 boolVector2;     // offset = 584,  size = 8,    align = 8
    bvec3 boolVector3;     // offset = 592,  size = 12,   align = 16
    bvec4 boolVector4;     // offset = 608,  size = 16,   align = 16
}; // 总大小 = 624 bytes

void main()
{
    TexCoord = InputTexCoord * uvScale;
    
    // 使用各种uniform确保它们不会被优化掉
    vec3 worldPos = vec3(iModel * vec4(InputPosition, 1.0));
    vec3 color = baseColor;
    
    for(int i = 0; i < 4; i++) {
        if(lightEnabled[i]) {
            vec3 lightPos = vec3(lightTransforms[i/2] * vec4(lightPositions[i], 1.0));
            color += lightColors[i] * lightIntensities[i];
        }
    }

    vec3 complexColor = baseColor;
    complexColor += vector3 * scalar;
    complexColor += vector4.xyz * intScalar;
    complexColor += vec3(matrix3 * vector3);
    complexColor += floatArray[2] * vec2Array[1].xyx;
    complexColor += lights[0].color.rgb * lights[0].intensity;
    complexColor += vec3(matrixArray[1] * vector2, 0.0);
    if(flag) {
        complexColor *= vec3(boolVector3.xyx);
    }
    
    gl_Position = iProjection * iView * vec4(worldPos + color * roughness, 1.0);
}
