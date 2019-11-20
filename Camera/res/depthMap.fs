#version 450 core
#extension GL_ARB_bindless_texture : require
// #define NUM_IMAGES 128 
#define NUM_IMAGES 1

in vec2 texCoords;

uniform float far_plane;
uniform float near_plane;
uniform int depthMap;

layout (std140, binding = 0) uniform IMAGE_BLOCK
{
	layout(r32ui) coherent uimage2D depthImage[NUM_IMAGES];
};

out vec4 FragColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}


void main() {
	float depthValue = float(imageLoad(depthImage[depthMap], ivec2(gl_FragCoord.xy)).x) ;
	depthValue = depthValue / 65534.0f;
	// FragColor  = vec4( vec3(depthValue ), 1.0);
	 FragColor  = vec4( vec3(LinearizeDepth(depthValue)/far_plane), 1.0);
	// if(FragColor == vec4(0.0 ,0.0, 0.0, 1.0))
	// 	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	
}

