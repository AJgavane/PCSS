#version 450 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;


void main(){
	gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);
}

