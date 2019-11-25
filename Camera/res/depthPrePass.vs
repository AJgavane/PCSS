#version 450 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
	gl_Position = projection * view * model * vec4(position, 1.0);
}

