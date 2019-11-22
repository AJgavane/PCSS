#version 450 core

in vec2 texCoords;

uniform sampler2D u_shadowMap;
uniform float light_zFar;
uniform float light_zNear;

out vec4 FragColor;

float zClipToEye(float z)
{
	return light_zFar * light_zNear / (light_zFar - z * (light_zFar - light_zNear));
}


void main()
{
	float z = ((texCoords.x <= 1.0) && (texCoords.y <= 1.0)) &&
		((texCoords.x >=0) && (texCoords.y >= 0)) ? texture(u_shadowMap, texCoords).z : 1.0;
	
	float color = (zClipToEye(z) - light_zNear) / (light_zFar - light_zNear)*10;
	FragColor = vec4(color, color, color, 1.0);
}
