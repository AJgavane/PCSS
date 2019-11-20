#version 450 core

in VS_OUT {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal;
uniform vec3 lightPosition;
uniform vec3 eye;


void main()
{
	vec3 lightColor = vec3(1.0);
	vec3 MaterialDiffuseColor = texture(texture_diffuse1, fs_in.TexCoord).rgb;
	vec3 MaterialAmbientColor = vec3(0.2);
	vec3 MaterialSpecularColor =texture(texture_specular1, fs_in.TexCoord).rgb;

	vec3 ambient = MaterialAmbientColor * MaterialDiffuseColor;

	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(lightPosition - fs_in.Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * MaterialDiffuseColor;

	vec3 viewDir = normalize(eye - fs_in.Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
	vec3 specular = spec * MaterialSpecularColor;

	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result, 1.0);
}