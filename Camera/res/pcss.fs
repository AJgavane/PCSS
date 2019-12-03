#version 450 core
#define EPSILON 0.00001
in VS_OUT {
	vec4 WorldPos;
	vec3 Normal;
	vec2 TexCoord;
	vec4 LightPosition;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform int u_samplePattern;
uniform sampler2D u_shadowMap;
uniform sampler2DShadow u_shadowMapPcf;

uniform vec3 u_lightPosition;
uniform mat4 u_lightView;
uniform vec3 u_eye;
uniform float u_light_zFar;
uniform float u_light_zNear;
uniform vec2 u_lightRadiusUV;

out vec4 FragColor;

#define POISSON_25_25   0
#define POISSON_32_64   1
#define POISSON_100_100 2
#define POISSON_64_128  3
#define REGULAR_49_225  4

const vec2 Poisson4[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

const vec2 Poisson25[25] = vec2[](
    vec2(-0.978698, -0.0884121),
    vec2(-0.841121, 0.521165),
    vec2(-0.71746, -0.50322),
    vec2(-0.702933, 0.903134),
    vec2(-0.663198, 0.15482),
    vec2(-0.495102, -0.232887),
    vec2(-0.364238, -0.961791),
    vec2(-0.345866, -0.564379),
    vec2(-0.325663, 0.64037),
    vec2(-0.182714, 0.321329),
    vec2(-0.142613, -0.0227363),
    vec2(-0.0564287, -0.36729),
    vec2(-0.0185858, 0.918882),
    vec2(0.0381787, -0.728996),
    vec2(0.16599, 0.093112),
    vec2(0.253639, 0.719535),
    vec2(0.369549, -0.655019),
    vec2(0.423627, 0.429975),
    vec2(0.530747, -0.364971),
    vec2(0.566027, -0.940489),
    vec2(0.639332, 0.0284127),
    vec2(0.652089, 0.669668),
    vec2(0.773797, 0.345012),
    vec2(0.968871, 0.840449),
    vec2(0.991882, -0.657338)
);
const vec2 Poisson128[128] = vec2[](
    vec2(-0.9406119, 0.2160107),
    vec2(-0.920003, 0.03135762),
    vec2(-0.917876, -0.2841548),
    vec2(-0.9166079, -0.1372365),
    vec2(-0.8978907, -0.4213504),
    vec2(-0.8467999, 0.5201505),
    vec2(-0.8261013, 0.3743192),
    vec2(-0.7835162, 0.01432008),
    vec2(-0.779963, 0.2161933),
    vec2(-0.7719588, 0.6335353),
    vec2(-0.7658782, -0.3316436),
    vec2(-0.7341912, -0.5430729),
    vec2(-0.6825727, -0.1883408),
    vec2(-0.6777467, 0.3313724),
    vec2(-0.662191, 0.5155144),
    vec2(-0.6569989, -0.7000636),
    vec2(-0.6021447, 0.7923283),
    vec2(-0.5980815, -0.5529259),
    vec2(-0.5867089, 0.09857152),
    vec2(-0.5774597, -0.8154474),
    vec2(-0.5767041, -0.2656419),
    vec2(-0.575091, -0.4220052),
    vec2(-0.5486979, -0.09635002),
    vec2(-0.5235587, 0.6594529),
    vec2(-0.5170338, -0.6636339),
    vec2(-0.5114055, 0.4373561),
    vec2(-0.4844725, 0.2985838),
    vec2(-0.4803245, 0.8482798),
    vec2(-0.4651957, -0.5392771),
    vec2(-0.4529685, 0.09942394),
    vec2(-0.4523471, -0.3125569),
    vec2(-0.4268422, 0.5644538),
    vec2(-0.4187512, -0.8636028),
    vec2(-0.4160798, -0.0844868),
    vec2(-0.3751733, 0.2196607),
    vec2(-0.3656596, -0.7324334),
    vec2(-0.3286595, -0.2012637),
    vec2(-0.3147397, -0.0006635741),
    vec2(-0.3135846, 0.3636878),
    vec2(-0.3042951, -0.4983553),
    vec2(-0.2974239, 0.7496996),
    vec2(-0.2903037, 0.8890813),
    vec2(-0.2878664, -0.8622097),
    vec2(-0.2588971, -0.653879),
    vec2(-0.2555692, 0.5041648),
    vec2(-0.2553292, -0.3389159),
    vec2(-0.2401368, 0.2306108),
    vec2(-0.2124457, -0.09935001),
    vec2(-0.1877905, 0.1098409),
    vec2(-0.1559879, 0.3356432),
    vec2(-0.1499449, 0.7487829),
    vec2(-0.146661, -0.9256138),
    vec2(-0.1342774, 0.6185387),
    vec2(-0.1224529, -0.3887629),
    vec2(-0.116467, 0.8827716),
    vec2(-0.1157598, -0.539999),
    vec2(-0.09983152, -0.2407187),
    vec2(-0.09953719, -0.78346),
    vec2(-0.08604223, 0.4591112),
    vec2(-0.02128129, 0.1551989),
    vec2(-0.01478849, 0.6969455),
    vec2(-0.01231739, -0.6752576),
    vec2(-0.005001599, -0.004027164),
    vec2(0.00248426, 0.567932),
    vec2(0.00335562, 0.3472346),
    vec2(0.009554717, -0.4025437),
    vec2(0.02231783, -0.1349781),
    vec2(0.04694207, -0.8347212),
    vec2(0.05412609, 0.9042216),
    vec2(0.05812819, -0.9826952),
    vec2(0.1131321, -0.619306),
    vec2(0.1170737, 0.6799788),
    vec2(0.1275105, 0.05326218),
    vec2(0.1393405, -0.2149568),
    vec2(0.1457873, 0.1991508),
    vec2(0.1474208, 0.5443151),
    vec2(0.1497117, -0.3899909),
    vec2(0.1923773, 0.3683496),
    vec2(0.2110928, -0.7888536),
    vec2(0.2148235, 0.9586087),
    vec2(0.2152219, -0.1084362),
    vec2(0.2189204, -0.9644538),
    vec2(0.2220028, -0.5058427),
    vec2(0.2251696, 0.779461),
    vec2(0.2585723, 0.01621339),
    vec2(0.2612841, -0.2832426),
    vec2(0.2665483, -0.6422054),
    vec2(0.2939872, 0.1673226),
    vec2(0.3235748, 0.5643662),
    vec2(0.3269232, 0.6984669),
    vec2(0.3425438, -0.1783788),
    vec2(0.3672505, 0.4398117),
    vec2(0.3755714, -0.8814359),
    vec2(0.379463, 0.2842356),
    vec2(0.3822978, -0.381217),
    vec2(0.4057849, -0.5227674),
    vec2(0.4168737, -0.6936938),
    vec2(0.4202749, 0.8369391),
    vec2(0.4252189, 0.03818182),
    vec2(0.4445904, -0.09360636),
    vec2(0.4684285, 0.5885228),
    vec2(0.4952184, -0.2319764),
    vec2(0.5072351, 0.3683765),
    vec2(0.5136194, -0.3944138),
    vec2(0.519893, 0.7157083),
    vec2(0.5277841, 0.1486474),
    vec2(0.5474944, -0.7618791),
    vec2(0.5692734, 0.4852227),
    vec2(0.582229, -0.5125455),
    vec2(0.583022, 0.008507785),
    vec2(0.6500257, 0.3473313),
    vec2(0.6621304, -0.6280518),
    vec2(0.6674218, -0.2260806),
    vec2(0.6741871, 0.6734863),
    vec2(0.6753459, 0.1119422),
    vec2(0.7083091, -0.4393666),
    vec2(0.7106963, -0.102099),
    vec2(0.7606754, 0.5743545),
    vec2(0.7846709, 0.2282225),
    vec2(0.7871446, 0.3891495),
    vec2(0.8071781, -0.5257092),
    vec2(0.8230689, 0.002674922),
    vec2(0.8531976, -0.3256475),
    vec2(0.8758298, -0.1824844),
    vec2(0.8797691, 0.1284946),
    vec2(0.926309, 0.3576975),
    vec2(0.9608918, -0.03495717),
    vec2(0.972032, 0.2271516)
);

bool isBlack(vec3 c)
{
    return (dot(c, c) == 0.0);
}

float biasedZ(float z0, vec2 dz_duv, vec2 offset)
{
    return z0 - dot(dz_duv, offset);
}

float zClipToEye(float z)
{
    return u_light_zFar * u_light_zNear / (u_light_zFar - z * (u_light_zFar - u_light_zNear));
}

float borderDepthTexture(sampler2D tex, vec2 uv)
{
	return ((uv.x <= 1.0) && (uv.y <= 1.0) && (uv.x >= 0.0) && (uv.y >= 0.0)) 
			? textureLod(tex, uv, 0.0).z : 1.0;
}

float borderPCFTexture(sampler2DShadow tex, vec3 uvz)
{
	return ((uvz.x <= 1.0) && (uvz.y <= 1.0) &&
	 (uvz.x >= 0.0) && (uvz.y >= 0.0)) ? texture(tex, uvz) : 
	 ((uvz.z <= 1.0) ? 1.0 : 0.0);
}

vec2 SerachRegionRadiusUV(float zWorld)
{
    return u_lightRadiusUV * (zWorld - u_light_zNear) / zWorld;
}

// Using similar triangles between the area light, the blocking plane and the surface point
vec2 penumbraRadiusUV(float zReceiver, float zBlocker)
{
    return u_lightRadiusUV * (zReceiver - zBlocker) / zBlocker;
}

// Project UV size to the near plane of the light
vec2 projectToLightUV(vec2 sizeUV, float zWorld)
{
    return sizeUV * u_light_zNear / zWorld;
}

vec2 depthGradient(vec2 uv, float z)
{
    vec2 dz_duv = vec2(0.0, 0.0);

    vec3 duvdist_dx = dFdx(vec3(uv,z));
    vec3 duvdist_dy = dFdy(vec3(uv,z));

    dz_duv.x = duvdist_dy.y * duvdist_dx.z;
    dz_duv.x -= duvdist_dx.y * duvdist_dy.z;

    dz_duv.y = duvdist_dx.x * duvdist_dy.z;
    dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

    float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
    dz_duv /= det;

    return dz_duv;
}

vec4 shading(vec3 worldPos, vec3 normal, float visibility)
{
	vec3 lightColor = vec3(1.0);
	// vec3 MaterialDiffuseColor = texture(texture_diffuse1, fs_in.TexCoord).rgb;
	// vec3 MaterialAmbientColor = vec3(0.2);
	// vec3 MaterialSpecularColor =texture(texture_specular1, fs_in.TexCoord).rgb;
    vec3 MaterialDiffuseColor = vec3(0.7); //texture(texture_diffuse1, fs_in.TexCoord ).rgb;
    vec3 MaterialAmbientColor = vec3(0.3);// * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.2);
	
    vec3 ambient = MaterialAmbientColor * MaterialDiffuseColor;
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(u_lightPosition - fs_in.WorldPos.xyz);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * MaterialDiffuseColor;

	vec3 viewDir = normalize(u_eye - fs_in.WorldPos.xyz);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
	vec3 specular = spec * MaterialSpecularColor;

	vec3 result = ambient + visibility * (diffuse + specular); 
	return vec4(result,1.0);
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void findBlocker(
    out float accumBlockerDepth, 
    out float numBlockers,
    out float maxBlockers,
    vec2 uv,
    float z0,
    vec2 dz_duv,
    vec2 searchRegionRadius)
{
    accumBlockerDepth = 0.0;
    numBlockers = 0.0;
	maxBlockers = 300.0;
    vec2 texelSize = 1.0f / textureSize(u_shadowMap, 0);
   // scale = getScale(fs_in.worldPos, z0);
     maxBlockers = 25.0;
    for (int i = 0; i < 25; ++i)
    {
        vec2 offset = Poisson25[i] * searchRegionRadius * texelSize;
        float shadowMapDepth = borderDepthTexture(u_shadowMap, uv + offset);
        float z = biasedZ(z0, dz_duv, offset);
        if (shadowMapDepth <= z)
        {
            accumBlockerDepth += shadowMapDepth;
            numBlockers += 1;
        }
    }
    
}

// Performs PCF filtering on the shadow map using multiple taps in the filter region.
float pcfFilter(vec2 uv, float z0, vec2 dz_duv, vec2 filterRadiusUV)
{
    float sum = 0.0;

     for (int i = 0; i < 4; ++i)
        {
            vec2 offset = Poisson4[i] * filterRadiusUV;//  * 0.0001;
            float z = biasedZ(z0, dz_duv, offset);
            sum += borderPCFTexture(u_shadowMapPcf, vec3(uv + offset, z));
        }
        return sum / 4.0;

  
}

vec2 SearchWidth(vec2 uvLightSize, float receiverDistance)
{
    vec2 texelSize = 1.0f / textureSize(u_shadowMap, 0);
    return uvLightSize * (receiverDistance - u_light_zNear) / u_eye.z;
}

float Blocker(vec2 shadowCoords, float z0)
{
    int blockers =  0;
    float avgBlockerDistance = 0;
    vec2 searchWidth = SearchWidth(u_lightRadiusUV, z0);
    for (int i = 0; i < 25; i++)
    {
        vec2 offset = Poisson25[i] * searchWidth;
        float z = texture(u_shadowMap, shadowCoords + offset).r;
        if(z < (z0 - 0.001)){
            blockers++;
            avgBlockerDistance += z;
        }
    }
    if(blockers > 0)
        return avgBlockerDistance/blockers;
    else 
        return -1.0;
}

float pcssShadow(vec2 uv, float z, vec2 dz_duv, float zEye)
{
	// Blocker serach
	float accumBlockerDepth, numBlockers, maxBlockers;
	vec2 serachRegionRadiusUV = SerachRegionRadiusUV(zEye);
	
    float avgBlockerDepth = Blocker(uv, z);

	if(avgBlockerDepth == -1.0)
		return 1.0;

	// Penumbra size
	float penumbraWidth = (z - avgBlockerDepth)/avgBlockerDepth;
    vec2 filterRadius = u_lightRadiusUV * (penumbraWidth * u_light_zNear/z);
	//filtering
	float result = pcfFilter(uv, z, dz_duv, filterRadius);

	return penumbraWidth;
}

float PCFShadow(vec2 uv, float z0, vec2 dz_duv, float zEye)
{
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(u_shadowMap, 0);
    float samples = 128;
    for (int i = 0; i < samples; ++i)
    {
        vec2 offset = Poisson128[i] * u_lightRadiusUV * texelSize;
        float z = biasedZ(z0, dz_duv, offset);
        float shadowMapDepth = borderPCFTexture(u_shadowMapPcf, vec3(uv + offset, z + EPSILON));
        shadow += shadowMapDepth;
    }
    return (shadow/samples);

    // float InShadow = 0.0f;
    // vec2 texelSize = 1.0f / textureSize(u_shadowMap, 0);
    // float samples = 4;
    // for (int i = 0; i < samples; ++i)
    // {
    //     vec2 offset = Poisson4[i] * u_lightRadiusUV * texelSize;
    //     float z = currentDepth_atXY - 0.001;
    //     float shadowMapDepth = borderPCFTexture(u_shadowMapPcf, vec3(uv + offset, z));
    //     InShadow += shadowMapDepth;
    // }
    // shadow = 1 - (InShadow/samples);
}

void main()
{
    // Shadow cords
	vec2 uv = fs_in.LightPosition.xy / fs_in.LightPosition.w;
	float currentDepth_atXY = fs_in.LightPosition.z  / fs_in.LightPosition.w;
     // float closestDepth_atXY = borderDepthTexture(u_shadowMap, uv + searchWidth);
    //float shadow = currentDepth_atXY - EPSILON > closestDepth_atXY ? 1.0 : 0.0;
     vec2 dz_duv = depthGradient(uv, currentDepth_atXY);
    // vec2 texelSize = 2.0f / textureSize(u_shadowMap, 0);
    // vec2 searchWidth = u_lightRadiusUV * (currentDepth_atXY - 0.1) / currentDepth_atXY;
    float zEye = -(u_lightView * fs_in.WorldPos).z;
    vec2 searchWidth = u_lightRadiusUV * (zEye - u_light_zNear)/ zEye;
    float accumBlockerDepth = 0.0f;
    float numBlockers = 0;
    int samples = 25;
    float sum = 0.0;
    for (int i = 0; i < samples; ++i)
    {
        vec2 offset = Poisson25[i] * searchWidth;
        float closestDepth = borderDepthTexture(u_shadowMap, uv + offset);
        // float z = currentDepth_atXY - 0.00017;
        float z = biasedZ(currentDepth_atXY, dz_duv, offset);
        if (closestDepth <= z)
        {
            accumBlockerDepth += closestDepth;
            numBlockers += 1;
        }
    }
    float shadow = 0.0;
    if(numBlockers >  0.0)
        shadow = accumBlockerDepth/numBlockers;

    float avgBlockerDepth = shadow;
    vec2 penumbraWidth =  u_lightRadiusUV *  vec2((zEye - avgBlockerDepth) / zEye);
    for (int i = 0; i < samples; ++i)
    {
        vec2 offset = Poisson25[i] * penumbraWidth;//  * 0.0001;
        // float z = currentDepth_atXY  - 0.00017;
        float z  = biasedZ(currentDepth_atXY, dz_duv, offset);
        // sum +=  borderDepthTexture(u_shadowMap, uv + offset);
        sum += borderPCFTexture(u_shadowMapPcf, vec3(uv + offset, z));
    }
    shadow = 1 - (sum / samples);
    


    // FragColor = shading(fs_in.WorldPos.xyz, fs_in.Normal, 1 -  shadow);

	// // Compute gradient using ddx/ddy before any branching
 //    vec2 dz_duv = depthGradient(uv, z);
 //    vec4 color = shading(fs_in.WorldPos.xyz, fs_in.Normal, 1.0);
	// float zEye = -(u_lightView * fs_in.WorldPos).z;
 //    float shadow = 1.0;
 //    //shadow = PCFShadow(uv, z, dz_duv, zEye) ;
 //    shadow = pcssShadow(uv, z, dz_duv, zEye) ;
    FragColor = shading(fs_in.WorldPos.xyz, fs_in.Normal, 1.0) * (1 - shadow);
}

// Start with hard sahdows.
// Use the serachWidth as an offset and render the hard shadows. 
// Test each step of pcss with hardsahdows.