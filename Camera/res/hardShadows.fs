#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D 		texture_diffuse1;
uniform sampler2D	u_shadowMap;

void main(){

	// Light emission properties
	vec4 LightColor = vec4(1.0);
	
	// Material properties
	vec4 MaterialDiffuseColor = texture( texture_diffuse1, UV );

	vec3 projCoords = ShadowCoord.xyz/ShadowCoord.w;
	float closestDepth = texture(u_shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth - 0.00001 > closestDepth ? 1.0 : 0.0;

	//float visibility = texture( u_shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) );

	color = (1 - shadow) * MaterialDiffuseColor * LightColor;

}