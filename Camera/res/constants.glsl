
#ifndef constants_glsl
#define constants_glsl

#define NUM_IMAGES 128

layout (std140, binding = 0) uniform IMAGE_BLOCK
{
	layout(r32ui) coherent uimage2D depthImage[NUM_IMAGES];
};

layout (std140, binding = 1) uniform LIGHT_BLOCK
{
    vec4 lightPosition[NUM_IMAGES];
};

layout (std140, binding = 2) uniform MVP_MATRIX_BLOCK
{
    mat4 lightMVP_mat[NUM_IMAGES];
};

layout (std140, binding = 4) uniform INV_MVP_MATRIX_BLOCK
{
    mat4 lightInvMVP_mat[NUM_IMAGES];
};

layout (std140, binding = 5) uniform VIEW_MATRIX_BLOCK
{
    mat4 lightView_mat[NUM_IMAGES];
};