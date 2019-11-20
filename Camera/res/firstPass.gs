#version 450
#extension GL_ARB_bindless_texture : require
#extension GL_NV_gpu_shader5 : enable
#extension GL_EXT_shader_image_load_store : enable
#extension GL_EXT_bindable_uniform : enable
#extension GL_NV_shader_buffer_load : enable

// #define NUM_IMAGES 128
#define NUM_IMAGES 1
#define FLT_MAX 3.402823466e+38
#define FLT_MIN -3.402823466e+38
#define M_PI 3.1415926535897932384626433832795
#define TAN_PI_BY_8 0.41421356237
#define pixelSize 0.001953125 
#define pixelSizeSq 0.000003814697265625
#define oneBy1024 0.0009765625
#define oneBy2048 0.00048828125
#define SQRT2 1.41421356237
#define EPSILON 0.00080901086
#define EPSILON_SQUARE 6.54498578e-7


layout (triangles, invocations = 1) in;
layout (triangle_strip, max_vertices = 3) out;

// uniform float f_minus_n;
// uniform float f_plus_n;
uniform int startImgIdx;
uniform int endImgIdx;
uniform mat4 VIR_perspMatrix;
uniform mat4 virView;
uniform bool countTri;
uniform float zNear;
uniform float pixLen;
uniform vec3 lightLookAt;

layout (std140, binding = 0) uniform IMAGE_BLOCK
{
	layout(r32ui) coherent uimage2D depthImage[NUM_IMAGES];
};

layout (std140, binding = 1) uniform LIGHT_BLOCK
{
    vec4 lightPosition[NUM_IMAGES];
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


layout (binding = 5) uniform atomic_uint numOfTriangles;

uint UnsignedDepth(float depth){
	uint val = uint((depth + 1.0) * 32767.0) + 1;
	return val;
}

in VS_OUT {
    vec4 worldPos;
    vec3 normal;
} gs_in[];

out GS_OUT {
   noperspective vec4 worldPos;
} gs_out;

vec3 GetCentroid(vec3 p0, vec3 p1, vec3 p2) {
    return (p0 + p1 + p2)*(1/3.0);
}

vec3 getNormal(vec3 p0, vec3 p1, vec3 p2){
	return normalize(cross(p2-p0, p1-p0) );
}

vec3 GetPerp(vec3 N) {		
    vec3 result = vec3(0.0);
    if(N.x == 0 && N.y == 0){ 
    		result.y = 1.0;
    } else {
    	result.x = -N.y;
    	result.y = N.x;
    }
    return normalize(result);
}

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { 
	return hash( v.x ^ hash(v.y)); 
}

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

float random( vec2  v ){ 
	return floatConstruct(hash(floatBitsToUint(v))); 
}

vec3 GetLargestDiagOfTri(vec3 p0, vec3 p1, vec3 p2){
	float dist01 = length(p0 - p1);
	float dist12 = length(p2 - p1);
	float dist02 = length(p0 - p2);
	if (dist01 < dist12)
		if(dist01 < dist02)
			return vec3((p2 - (p0 + p1)*0.5).xy, 0);
		else 
			return vec3((p1 - (p2 + p0)* 0.5).xy,2);
			
	else 
		if (dist12 < dist02)
			return vec3((p0 - (p1 + p2) * 0.5).xy, 1);
		else 
			return vec3((p1 - (p2 + p0) * 0.5).xy,2);
	
}

float areaOfSquare(vec3 p1, vec3 p2, vec3 p3, vec3 p4){
	vec3 diag1 = (p3 - p1);
	vec3 diag2 = (p4 - p2);
	float area = length(cross(diag1, diag2)) * 0.5;
	return area;
}

float areaOfTriangle(vec4 p1, vec4 p2, vec4 p3){
	vec3 side1 = (p3 - p1).xyz;
	vec3 side2 = (p2 - p1).xyz;
	float area = length(cross(side1, side2)) * 0.5;
	return area;
}

float areaOfTriangle(vec3 p1, vec3 p2, vec3 p3){
	vec3 side1 = (p3 - p1);
	vec3 side2 = (p2 - p1);
	float area = length(cross(side1, side2)) * 0.5;
	return area;
}

vec3 FindFarthestPoint(vec3 p0, vec3 p1, vec3 p2,vec3 src) 
{
	float dist0 = length(src - p0);
	float dist1 = length(src - p1);
	float dist2 = length(src - p2);
	if(dist0 > dist1)
		if(dist0 > dist2)
			return p0;
		else 
			return p2;
	else 
		if(dist1 > dist2)
			return p1;
		else 
			return p2;
}

vec3 findClosestPoint(vec3 a, vec3 b, vec3 c, vec3 p){
	// Check if p in vertex region outside A
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 ap = p - a;
	float d1 = dot(ab, ap);
	float d2 = dot(ac, ap);
	if( d1 <= 0.0f && d2 <= 0.0f) return a;

	// check if p in vertex region outside B
	vec3 bp = p - b;
	float d3 = dot(ab, bp);
	float d4 = dot(ac, bp);
	if(d3 >= 0.0f && d4 <= d3) return b;

	// Check if P in edge region of AB if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if(vc <= 0.0f && d1>= 0.0f && d3 <= 0.0f){
		float v = d1 / (d1 - d3);
		return a + v * ab;
	}

   	// Check if P in vertex region outside C
   	vec3 cp = p - c;
   	float d5 = dot(ab, cp);
   	float d6 = dot(ac, cp);
   	if(d6 >= 0.0f && d5 <= d6) return c;

   	// Check if P in edge region of AC, if so return projection of P onto AC
   	float vb = d5*d2 - d1*d6;
   	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f){
   		float w = d2 / (d2 - d6);
   		return a + w * ac;
   	}

   	// Check if P in edge region of BC, if so return projection of P onto AC
   	float va = d3*d6 - d5*d4;
   	if(va <= 0.0f && (d4-d3) >= 0.0f && (d5-d6) >= 0.0f){
   		float w = (d4-d3)/((d4-d3) + (d5-d6));
   		return b + w * (c-b);
   	}

   	// P inside face region. 
   	float denom = 1.0f / (va + vb + vc);
   	float v = vb * denom;
   	float w = vc * denom;
   	return a + ab * v + ac * w;
}

void emit(int i, vec4 vertex){
	gs_out.worldPos = gs_in[i].worldPos;
    gl_Position = vertex;
    EmitVertex();
}


void emit(vec3 C, vec4 vertex, int i){
	gs_out.worldPos = gs_in[i].worldPos;
    gl_Position = vertex;
    EmitVertex();
}

void emit(int i, vec4 vertex, vec3 virCentroid, float delta){
	gs_out.worldPos = gs_in[i].worldPos;
    gl_Position = vertex + vec4(delta-virCentroid.x, -virCentroid.y , 0.0, 0.0);
    // gl_Position = vertex;
    // gl_Position = vertex;// + vec4(oneBy1024-virCentroid.x, oneBy1024-virCentroid.y , 0.0, 0.0);
    EmitVertex();
}

// Adam's method
float AdamsMethod(vec3 p0, vec3 p1, vec3 p2)
{
	float perspDistortion = 1.34;
	float minDist = FLT_MAX;
	for( int i = startImgIdx;  i < endImgIdx ; i++) 
	{	  
		/*Method - 1 */
		vec3 closestPoint = findClosestPoint(p0, p1, p2, lightPosition[i].xyz);
    	float sd =  distance(closestPoint.xyz, lightPosition[i].xyz);
		minDist = min(minDist, sd);	
	}
    float desity =   1.0 / ( TAN_PI_BY_8 * (minDist));
	return desity;
}

// Returns density = 1
float Method2(vec3 p0, vec3 p1, vec3 p2)
{
	return 1.0;
}

// Projected Area method
vec2 ProjectedAreaMethod(vec3 p0, vec3 p1, vec3 p2)
{
	//vec2 density = vec2(FLT_MAX);
	float minProjArea = FLT_MAX;
	for(int i = startImgIdx; i < endImgIdx; ++i)
	{
		vec4 closestPoint = vec4(findClosestPoint(p0, p1, p2, lightPosition[i].xyz), 1.0);
		vec4 closestPoint_view = lightView_mat[i] * closestPoint;
		vec4 closestPoint_proj = lightProj_mat[i] * closestPoint_view;
		// vec4 closestPoint_ndc = closestPoint_proj/closestPoint_proj.w;
		float delta = EPSILON * closestPoint_proj.w;
		vec4 deltaShift = lightInvProj_mat[i] * vec4(delta, delta, 0, 0);
		/** n1 */
		/* vec4 n1_ndc = closestPoint_ndc + vec4(pixelSize, 0, 0, 0);
		 * vec4 n1_proj = n1_ndc * closestPoint_proj.w;*/
		// vec4 n1_view = closestPoint_view - vec4(deltaShift.x, 0, 0, 0);
		/*n2*/
		// vec4 n2_view = closestPoint_view - vec4(deltaShift.xy, 0, 0);
		/*n3*/
		// vec4 n3_view = closestPoint_view -  vec4(0, deltaShift.y, 0, 0);
		
		// float area = areaOfSquare(closestPoint_view.xyz, n1_view.xyz, n2_view.xyz, n3_view.xyz);	// area of projected square
		float area = deltaShift.x * deltaShift.y;
		minProjArea = min(minProjArea, area);	// get minimum
		// density = vec2(min(density.x, deltaShift.x), min(density.y, deltaShift.y));
	}
	float density = sqrt(EPSILON_SQUARE/minProjArea); // get density
	return vec2(density);
	// return vec2(EPSILON/density.x, EPSILON/density.y);
}

void main(void)
{
	int clip = 1;
	bool clipPerView[NUM_IMAGES] = bool[NUM_IMAGES](true);

	vec3 p0 = (gs_in[0].worldPos).xyz;
    vec3 p1 = (gs_in[1].worldPos).xyz;
    vec3 p2 = (gs_in[2].worldPos).xyz; 
 	vec3 N = cross(p2-p0, p1-p0);	
    for (int i = startImgIdx; i < endImgIdx && i < NUM_IMAGES; i++)
	{
		vec3 view = lightPosition[i].xyz - p0;
		if(dot(view,N) <= 0.0){
			clip = 0;
			break;
		}   	
	}
	if(clip == 1)	// if back face return.
		return;
	    // 1. Centroid	    
	    vec3 C = GetCentroid(p0, p1, p2);// + vec3(oneBy1024,oneBy1024, 0.0);
	    // 2. Basis Frame	 
	    N = normalize(N);  
	    vec3 Up = GetPerp(N);			// Some perp vector to N
	    vec3 V = cross(N, Up);			// side vector

	    mat4 VIR_o = mat4(	Up.x, V.x, N.x, 0.0, 
							Up.y, V.y, N.y, 0.0,
							Up.z, V.z, N.z, 0.0,
							-dot(Up,C), -dot(V,C), -dot(N,C), 1.0);
	   
	    mat4 VIR;
		float virPix = pixelSize;
		bool orthoProj = true;
		if(orthoProj)
		{			
		    vec2 pixelDensity = ProjectedAreaMethod(p0, p1, p2);
		    mat4 proj = mat4(	pixelDensity.x, 0.0, 0.0, 0.0,
	    						0.0, pixelDensity.y, 0.0, 0.0,
	    						0.0, 0.0, 1.0, 0.0,
	    						0.0, 0.0, 0.0, 1.0 ); 
		     VIR = proj * VIR_o;
		} 
		else   
		{
	    	float w = FLT_MAX;  
		    for(int i = startImgIdx; i < endImgIdx && i < NUM_IMAGES; i++){
		    	vec3 L = lightPosition[i].xyz;
	    		vec4 farthestpoint = vec4(FindFarthestPoint(p0, p1, p2, L),1.0);

		   		vec3 vfp_cs = (lightView_mat[i] * farthestpoint).xyz;	   		
		   		vec3 imagePlaneFPIsect = -1 * vfp_cs/vfp_cs.z;	

			    float mxy_sq = pow(length(imagePlaneFPIsect),2);
		    	w = clamp(min(w, 1/mxy_sq),0.1, 2.0);
		    }
		    float smv = FLT_MAX;
		    float dv = 0;		  
		   for(int i = startImgIdx;  i < endImgIdx && i < NUM_IMAGES; i++){
		   		vec3 src = (lightPosition[i]).xyz;
		   		dv = length( src - (findClosestPoint(p0, p1, p2, src)).xyz);
	            float smv_i = w * dv  ;   // Find the smallest distance from view to the t/f tri.
		        smv = min(smv, smv_i);	// update the min.
		    }    
		    mat4 VIR_p = (VIR_o);	
		    VIR_p[3] = VIR_p[3] - vec4(0 , 0 , smv-1, 0);

		    VIR = VIR_perspMatrix * VIR_p;
	    }



		vec4 p_virOrtho[3];    
		p_virOrtho[0] = VIR * gs_in[0].worldPos;
	    p_virOrtho[1] = VIR * gs_in[1].worldPos;
	    p_virOrtho[2] = VIR * gs_in[2].worldPos;
	    float areaOfTri = areaOfTriangle(p_virOrtho[0], p_virOrtho[1], p_virOrtho[2]);
	    float areaOfVIRpix = virPix * virPix * 0.25 ;

	    float maxEdge = max(distance(p_virOrtho[0], p_virOrtho[1]),
	    					max(distance(p_virOrtho[1], p_virOrtho[2]),distance(p_virOrtho[0], p_virOrtho[2]))  );
    	vec3 diag = GetLargestDiagOfTri(p_virOrtho[0].xyz, p_virOrtho[1].xyz, p_virOrtho[2].xyz );
    	int shortEdgeIdx = int(diag.z);
	    float lenDiag = length(diag.xy);
	    float odds = max(2*areaOfTri/areaOfVIRpix, 0.0);
	    if(lenDiag <= virPix && random(N.xy) <= odds){	    	  // sub pixel
	    	vec4 points[3];
        	points[0] = vec4(oneBy2048, oneBy2048, 0, 1.0);
        	points[1] = vec4(2*oneBy2048, 3*oneBy2048, 0, 1.0);
        	points[2] = vec4(3*oneBy2048, oneBy2048, 0, 1.0);
        	emit(C, points[0], 0);
        	emit(C, points[1], 1);
        	emit(C, points[2], 2);		    
		    EndPrimitive();
			// atomicCounterIncrement(numOfTriangles);		    
		    return;
	    }  
	    else if (lenDiag >  virPix) {			// supra pixel
		  /*  vec2 normDiag = normalize(diag.xy);
		    float c = normDiag.y; 
		    float s = normDiag.x;
		    mat4 rot;
		    rot[0] = vec4(c, s, 0.0, 0.0);
		    rot[1] = vec4(-s, c, 0.0, 0.0);
		    rot[2] = vec4(0.0, 0.0, 1.0, 0.0);
		    rot[3] = vec4(0.0, 0.0, 0.0, 1.0);
		    mat4 scale; float scaleFact = 1.0;// - 0.5*oneBy2048/lenDiag;
		    scale[0] = vec4(scaleFact, 0.0, 0.0, 0.0);
		    scale[1] = vec4(0.0, scaleFact, 0.0, 0.0);
		    scale[2] = vec4(0.0, 0.0, 1.0, 0.0);
		    scale[3] = vec4(0.0, 0.0, 0.0, 1.0);
			p_virOrtho[0] = scale * rot * p_virOrtho[0];
			p_virOrtho[1] = scale * rot * p_virOrtho[1];
			p_virOrtho[2] = scale * rot * p_virOrtho[2];			
		    vec3 virCentroid = (p_virOrtho[shortEdgeIdx].xyz + p_virOrtho[(shortEdgeIdx+1)%2].xyz)*(0.5);
		    // vec3 virCentroid = (p_virOrtho[0].xyz + p_virOrtho[1].xyz + p_virOrtho[2].xyz)*(1/3.0);
        	emit(0, p_virOrtho[0], virCentroid, virPix);
        	emit(1, p_virOrtho[1], virCentroid, virPix);
        	emit(2, p_virOrtho[2], virCentroid, virPix);		    
		    EndPrimitive();*/
		    float w = FLT_MIN;  
		    for(int i = startImgIdx; i < endImgIdx && i < NUM_IMAGES; i++){
		    	vec3 L = lightPosition[i].xyz;
	    		vec4 farthestpoint = vec4(FindFarthestPoint(p0, p1, p2, L),1.0);

		   		vec4 vfp_proj = (lightView_mat[i] * farthestpoint);	   		
		   		vec2 imagePlaneFPIsect =  vfp_proj.xy/vfp_proj.z;	

			    float mxy_sq = 1.0 + pow(imagePlaneFPIsect.x,2) + pow(imagePlaneFPIsect.y,2);
		    	w = clamp(max(w, mxy_sq), 1.0, 1.34);
		    }
		    // vec2 normDiag = normalize(diag.xy);
		    // float c = normDiag.y; 
		    // float s = normDiag.x;
		    // mat4 rot;
		    // rot[0] = vec4(c, s, 0.0, 0.0);
		    // rot[1] = vec4(-s, c, 0.0, 0.0);
		    // rot[2] = vec4(0.0, 0.0, 1.0, 0.0);
		    // rot[3] = vec4(0.0, 0.0, 0.0, 1.0);
		    mat4 scale; float scaleFact = w;
		    scale[0] = vec4(scaleFact, 0.0, 0.0, 0.0);
		    scale[1] = vec4(0.0, scaleFact, 0.0, 0.0);
		    scale[2] = vec4(0.0, 0.0, 1.0, 0.0);
		    scale[3] = vec4(0.0, 0.0, 0.0, 1.0);
		    p_virOrtho[0] = scale /** rot*/ * p_virOrtho[0];
			p_virOrtho[1] = scale /** rot*/ * p_virOrtho[1];
			p_virOrtho[2] = scale /** rot*/ * p_virOrtho[2];			
		    // vec3 virCentroid = (p_virOrtho[shortEdgeIdx].xyz + p_virOrtho[(shortEdgeIdx+1)%2].xyz)*(0.5);
		    vec3 virCentroid = (p_virOrtho[0].xyz + p_virOrtho[1].xyz + p_virOrtho[2].xyz)*(1/3.0);
        	emit(0, p_virOrtho[0], virCentroid, virPix);
        	emit(1, p_virOrtho[1], virCentroid, virPix);
        	emit(2, p_virOrtho[2], virCentroid, virPix);		    
		    EndPrimitive();
		    if(countTri)
				atomicCounterIncrement(numOfTriangles);		     
			return;
	    } // */
}
