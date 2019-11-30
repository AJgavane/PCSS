#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 ShadowCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_depthBiasMVP;

void main()
{

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  u_projection * u_view * u_model * vec4(position,1);
	
	ShadowCoord = u_depthBiasMVP * u_model * vec4(position,1);
	
	// UV of the vertex. No special space for this one.
	UV = texCoord;
}

