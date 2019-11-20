#version 450 
#extension GL_ARB_bindless_texture : require
layout (early_fragment_tests) in;
// #define NUM_IMAGES 128 
#define NUM_IMAGES 1

in VS_OUT {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform int height;
uniform vec3 eye;
uniform int startImgIdx;
uniform int endImgIdx;

layout (std140, binding = 0) uniform IMAGE_BLOCK
{
    layout(r32ui) coherent uimage2D depthImage[NUM_IMAGES];
};

layout (std140, binding = 1) uniform LIGHT_BLOCK
{
    vec3 lightPosition[NUM_IMAGES];
};

layout (std140, binding = 2) uniform MVP_MATRIX_BLOCK
{
    mat4 lightMVP_mat[NUM_IMAGES];
};
layout (std140, binding = 4) uniform INV_MVP_MATRIX_BLOCK
{
    mat4 lightInvProj_mat[NUM_IMAGES];
};
layout (std140, binding = 5) uniform VIEW_MATRIX_BLOCK
{
    mat4 lightView_mat[NUM_IMAGES];
};
layout (std140, binding = 6) uniform PROJECTION_MATRIX_BLOCK
{
    mat4 lightProj_mat[NUM_IMAGES];
};
float FloatDepth(float depth){
    // float val = float(depth) / 65534.0;
    float val = float(depth) / 65533.0;
    return val;
}

float ShadowCalculation(vec4 fragPosLightSpace, float bias, int light)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = (projCoords + 1) *0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)

    ivec2 searchProjCoords = ivec2(projCoords.xy * height);
    vec4 depthVal = imageLoad(depthImage[light], searchProjCoords);
    float closestDepth = FloatDepth(depthVal.x);

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main() {
   
    vec3 lightColor = vec3(1.0);
    vec3 MaterialDiffuseColor = vec3(0.7); //texture(texture_diffuse1, fs_in.TexCoord ).rgb;
    vec3 MaterialAmbientColor = vec3(0.2);// * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.2);
    // ambient
    // diffuse
    float bias = 0.0;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    float shadow = 0.0f;

    vec3 normal = fs_in.Normal;
    for(int i = 0;  i < NUM_IMAGES; i++){
         //Light 1
        vec3 lightDir = normalize(lightPosition[i] - fs_in.Position);
        float diff = clamp(dot(lightDir, normal), 0.0, 1.0);
            diffuse += diff * lightColor * MaterialDiffuseColor;
            // specular
            vec3 viewDir = normalize(eye - fs_in.Position);
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = 0.0;
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            spec = pow(clamp(dot(normal, halfwayDir), 0.0, 1.0), 64.0);
            specular += spec * lightColor * MaterialSpecularColor; 
            // shadow
            float bias_acne = 0.01 * tan(acos(diff));  
            float bias1 = clamp(bias_acne, 0.0, 0.0005);
            bias = max(bias, bias1);
            vec4 temp = lightMVP_mat[i] * vec4(fs_in.Position, 1.0);
            shadow = shadow + ShadowCalculation(temp, bias, i);  
    }
    specular = (specular)/NUM_IMAGES;
    diffuse = (diffuse)/NUM_IMAGES;
    shadow = shadow / NUM_IMAGES;

    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    vec3 lighting = MaterialAmbientColor +
                    (1-shadow) * (diffuse + specular) ;    

    
    FragColor = vec4(lighting, 1.0);
}

