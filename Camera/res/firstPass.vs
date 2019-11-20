#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out VS_OUT {
	vec4 worldPos;
	vec3 normal;
	// vec4 projectedPos;
} vs_out;

void main() 
{
	vs_out.worldPos =  model * vec4(position, 1.0);
	vs_out.normal = transpose(inverse(mat3(model))) * normal;
	// vs_out.projectedPos = projection * view * vs_out.worldPos;
	gl_Position = vs_out.worldPos;
}