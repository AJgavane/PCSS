#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;


out VS_OUT {
	vec4 WorldPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 LightPosition;
} vs_out;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_depthBiasMVP;

void main()
{
	vs_out.WorldPos = u_model * vec4(position, 1.0);
	vs_out.Normal = normalize(transpose(inverse(mat3(u_model))) * normal);
	vs_out.TexCoord = texCoord;
	vs_out.LightPosition = u_depthBiasMVP * vs_out.WorldPos;
	gl_Position = u_projection * u_view * vs_out.WorldPos;
}