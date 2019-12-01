#version 450 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 ShadowCoord;
in vec4 WorldPos;
in vec3 Normal;
// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D	u_shadowMap;
uniform vec3 u_lightPosition;
uniform vec3 u_eye;

vec4 shading(vec3 worldPos, vec3 normal, vec2 texCoord, float visibility)
{
	vec3 lightColor = vec3(1.0);
	vec3 MaterialDiffuseColor = texture(texture_diffuse1, texCoord).rgb;
	vec3 MaterialAmbientColor = vec3(0.2);
	vec3 MaterialSpecularColor =texture(texture_specular1,texCoord).rgb;

	vec3 ambient = MaterialAmbientColor * MaterialDiffuseColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(u_lightPosition - worldPos.xyz);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * MaterialDiffuseColor;

	vec3 viewDir = normalize(u_eye - worldPos.xyz);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
	vec3 specular = spec * MaterialSpecularColor;

	vec3 result = ambient + visibility * (diffuse + specular); 
	return vec4(result,1.0);
}

void main(){

	// Light emission properties
	vec4 LightColor = vec4(1.0);
	
	// Material properties
	vec4 MaterialDiffuseColor = texture( texture_diffuse1, UV );

	vec3 projCoords = ShadowCoord.xyz/ShadowCoord.w;
	float closestDepth = texture(u_shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth - 0.0001 > closestDepth ? 1.0 : 0.0;

	//float visibility = texture( u_shadowMap, vec3(ShadowCoord.xy, (ShadowCoord.z)/ShadowCoord.w) );

	color = shading(WorldPos.xyz, Normal, UV, 1 - shadow);

}