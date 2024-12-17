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

struct NestedStruct {
    mat3x4 transform;    // 内部偏移 = 0,   大小 = 48  (3列,每列vec4)
    vec2 parameters[2];  // 内部偏移 = 48,  大小 = 32  (数组元素16字节对齐)
};                      // 总大小 = 80,     对齐 = 16

struct ComplexLight {
    vec4 position;       // 内部偏移 = 0,   大小 = 16
    mat2 rotation;       // 内部偏移 = 16,  大小 = 32  (2列,每列vec2,16字节对齐)
    float[3] factors;    // 内部偏移 = 48,  大小 = 12  (数组元素4字节对齐)
};                      // 总大小 = 64,     对齐 = 16

layout(std140, binding = 4) uniform ExtendedBlock
{
    // 基本类型和数组
    double dScalar;          // offset = 0,    size = 8,    align = 8
    dvec2 doubleVector2;     // offset = 8,    size = 16,   align = 16
    dvec3 doubleVector3;     // offset = 32,   size = 24,   align = 32
    dvec4 doubleVector4;     // offset = 64,   size = 32,   align = 32
    
    // 矩阵数组
    mat4x3 mat4x3Array[2];   // offset = 96,   size = 128,  align = 16 (2个mat4x3,每个64字节)
    mat3x2 mat3x2Array[3];   // offset = 224,  size = 144,  align = 16 (3个mat3x2,每个48字节)
    
    // 嵌套结构体
    NestedStruct nested;     // offset = 368,  size = 80,   align = 16
    
    // 结构体数组
    ComplexLight clights[2]; // offset = 448,  size = 128,  align = 16 (2个ComplexLight,每个64字节)
    
    // 布尔数组和矩阵
    bvec4 boolArray[3];      // offset = 576,  size = 48,   align = 16 (3个bvec4)
    dmat2 doubleMatrix2;     // offset = 624,  size = 64,   align = 16 (2列double vec2)
    dmat3 doubleMatrix3;     // offset = 688,  size = 144,  align = 32 (3列double vec3)
    
    // 混合类型数组
    struct {
        vec4 color;          // 内部偏移 = 0,   大小 = 16
        bvec3 flags;         // 内部偏移 = 16,  大小 = 12,  对齐到16
        float intensity;     // 内部偏移 = 28,  大小 = 4
    } mixedData[2];          // offset = 832,  size = 64,   align = 16 (2个32字节结构体)
    
    // 特殊矩阵类型
    dmat2x4 dmatrix2x4;     // offset = 896,  size = 64,   align = 32 (2列double vec4)
    dmat4x2 dmatrix4x2;     // offset = 960,  size = 128,  align = 32 (4列double vec2)
    
    // 原子计数器(只能在std430布局中使用)
    // atomic_uint counter;  // 不支持在std140中使用
    
    uint padding;           // offset = 1088, size = 4,    align = 4
}; // 总大小 = 1092 bytes

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
