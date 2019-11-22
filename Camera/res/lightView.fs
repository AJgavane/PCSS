#version 450 core

uniform sampler2D shadowMap;
uniform float light_zNear;
uniform float light_zFar;

in vec2 texCoords;
out vec4 FragColor;

float zClipToEye(float z)
{
	return light_zFar * light_zNear / (light_zFar - z * (light_zFar - light_zNear));
}


void main()
{
	float z = ((texCoords.x <= 1.0) && (texCoords.y <= 1.0)) &&
		((texCoords.x >=0) && (texCoords.y >= 0)) ? texture(shadowMap, texCoords).z : 1.0;
	
	float color = (zClipToEye(z) - light_zNear) / (light_zFar - light_zNear) * 10.0;
	FragColor = vec4(color, 0.0, 0.0, 1.0);
	//FragColor = vec4(color, color, color, 1.0);
	//FragColor = vec4(0.20, 0.0, 0.0, 1.0);
}