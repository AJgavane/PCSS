#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;


out vec2 texCoords;

void main()
{
	texCoords = texCoord;
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}