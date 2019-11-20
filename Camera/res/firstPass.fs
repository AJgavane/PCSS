#version 450 
#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : enable
precision highp float;
// #define NUM_IMAGES 128 
#define NUM_IMAGES 1 

in GS_OUT {
    noperspective vec4 worldPos;
} fs_in;

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
    mat4 lightInvProj_mat[NUM_IMAGES];
};
layout (std140, binding = 5) uniform VIEW_MATRIX_BLOCK
{
    mat4 lightView_mat[NUM_IMAGES];
};
layout (std140, binding = 6) uniform PROJECTION_MATRIX_BLOCK
{
    mat4 lightProj_mat[NUM_IMAGES];
};
layout (binding = 3) uniform atomic_uint numOfPoints;

// uniform int height_by_2;
uniform int startImgIdx;
uniform int endImgIdx;
uniform bool pointCounter;
out vec4 color;

uint UnsignedDepth(float depth){
	uint val = uint((depth + 1.0) * 32767.5) ;
	return val;
}

void main()
{
	int height_by_2 = 512;	
	for(int i = startImgIdx; i< endImgIdx && i < NUM_IMAGES; i++){
			vec4 projCoord_light = lightMVP_mat[i] * fs_in.worldPos;		
			vec3 projCoordNDC_light = projCoord_light.xyz/ projCoord_light.w;
			if( projCoordNDC_light.x >= -1 && projCoordNDC_light.x <= 1 && 
				projCoordNDC_light.y >= -1 && projCoordNDC_light.y <= 1 && 
				projCoordNDC_light.z >= 0 && projCoordNDC_light.z <= 1 ) {
				imageAtomicMin(depthImage[i], ivec2((projCoordNDC_light.xy + 1.0) * height_by_2),  UnsignedDepth(projCoordNDC_light.z));
				// imageStore(depthImage[i], ivec2((projCoordNDC_light.xy + 1.0) * height_by_2),  uvec4(UnsignedDepth(projCoordNDC_light.z)));
			}							
	}
	if(pointCounter)
		atomicCounterIncrement(numOfPoints);
	color = vec4(1.0, 0.0, 0.0, 1.0);
}