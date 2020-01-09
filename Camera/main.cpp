#include <iostream>
#include<vector>
#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "camera.h"
#include "Constants.h"
#include "HandelKeys.h"
#include <GL/GL.h>
#include <GL/glext.h>

void genQueries(GLuint qid[][1]);
void swapQueryBuffers();
bool initShadowMap(int unit);
void BindFBOForWriting();
void BindFBOForReading(GLenum TextureUnit);
void DrawQuadGL();
void initRendering();
void initTexutures();

inline glm::vec3 transformCoord(const glm::mat4 &m, const glm::vec3 &v)
{
	glm::vec4 r = m * glm::vec4(v, 1.0);
	float oow = r.w == 0.0 ? 1.0 : (1.0 / r.w);
	return glm::vec3(r) * oow;
}

inline void transformBoundingBox(glm::vec3 bbox2[2], const glm::vec3 bbox1[2], const glm::mat4x4 &matrix)
{
	bbox2[0][0] = bbox2[0][1] = bbox2[0][2] = std::numeric_limits<float>::max();
	bbox2[1][0] = bbox2[1][1] = bbox2[1][2] = -std::numeric_limits<float>::max();
	// Transform the vertices of BBox1 and extend BBox2 accordingly
	for (int i = 0; i < 8; ++i)
	{
		glm::vec3 v(
			bbox1[(i & 1) ? 0 : 1][0],
			bbox1[(i & 2) ? 0 : 1][1],
			bbox1[(i & 4) ? 0 : 1][2]);

		glm::vec3 v1 = transformCoord(matrix, v);
		for (int j = 0; j < 3; ++j)
		{
			bbox2[0][j] = std::min(bbox2[0][j], v1[j]);
			bbox2[1][j] = std::max(bbox2[1][j], v1[j]);
		}
	}
}

int main(int args, char** argv)
{
    float dTheta = 0.01;
	std::fill(&m_samplers[0], &m_samplers[NumTextureUnits], 0);
	std::fill(&m_textures[0], &m_textures[NumTextureUnits], 0);

	Model floor("./res/models/floor/floor.obj");
    glm::mat4 modelFloor;
    modelFloor = glm::translate(modelFloor, glm::vec3(0.00f, -1.00f, 1.00f));
    modelFloor = glm::translate(modelFloor, -floor.GetCenter());
    modelFloor = glm::scale(modelFloor, glm::vec3(0.30f, 1.0f, 0.3f));	// it's a bit too big for our scene, so scale it down

	Model wall("./res/models/wall/wall.obj");
	glm::mat4 modelWall;
	modelWall = glm::rotate(modelWall, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelWall = glm::translate(modelWall, glm::vec3(0.0, 3.5, -1.0  ));
	modelWall = glm::scale(modelWall, glm::vec3(0.30f, 1.0, 0.3));	// it's a bit too big for our scene, so scale it down

	glm::mat4 modelScene;
	Model scene("./res/models/scene/scene.obj");
	modelScene = glm::translate(modelScene, glm::vec3(0.50f, -.800f, 2.0f));

	runtime = true; avgNumFrames = int(glm::radians(360.0f) * 20); csv = true;  dTheta = 0;
	int sampling = SamplePattern::POISSON_100_100;
  
	initRendering();

	glm::mat4 ModelLight;
	Model light("./res/models/square/square.obj");
	ModelLight = glm::mat4();
	ModelLight = glm::scale(ModelLight, glm::vec3(LIGHT_SIZE*10.0f, LIGHT_SIZE*10.0f, 1.0f));	
	ModelLight = glm::translate(ModelLight, lightPosition);
	
	glm::mat4 lightView = glm::lookAt(lightPosition, lightLookAt, glm::vec3(0.0f, 1.0f, 0.0f)); 
	glm::vec3 center = scene.GetCenter();
	glm::vec3 extents = scene.GetExtent();
	glm::vec3 box[2];
	box[0] = center - extents;
	box[1] = center + extents;
	glm::vec3 bboxScaled[2];
	transformBoundingBox(bboxScaled, box, lightView * modelScene);
	
	FrustumWidth = std::max(fabs(bboxScaled[0][0]), fabs(bboxScaled[1][0])) * 2.0f;
	FrustumHeight =  std::max(fabs(bboxScaled[0][1]), fabs(bboxScaled[1][1])) * 2.0f;//*/
	l_zNear = -bboxScaled[1][2] - 2.9;
	

	glm::mat4 lightProj = glm::perspective(lightFOV, 1.0f, l_zNear, zFar);
	glm::mat4 lightViewProj = lightProj * lightView;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 depthBiasMVP = biasMatrix * lightViewProj;

	// Load shaders	
	Shader shadowMapShader("./res/shadowMap.vs", "./res/shadowMap.fs");
	shadowMapShader.use();
	shadowMapShader.setMat4("u_viewProj", lightViewProj);
	shadowMapShader.disable();
	
	Shader lightViewShader("./res/lightView.vs", "./res/lightView.fs");
	lightViewShader.use();
	lightViewShader.setFloat("light_zFar", zFar);
	lightViewShader.setFloat("light_zNear", l_zNear);
	lightViewShader.setInt("u_shadowMap", 0);
	lightViewShader.disable();

	Shader pcss("./res/pcss.vs", "./res/pcss.fs");
	pcss.use();
	pcss.setMat4("u_depthBiasMVP", depthBiasMVP);
	pcss.setVec3("u_lightPosition", lightPosition);
	pcss.setMat4("u_lightView", lightView);
	pcss.setFloat("u_light_zNear", l_zNear);
	pcss.setFloat("u_light_zFar", zFar);
	pcss.setVec2("u_lightRadiusUV", glm::vec2(LIGHT_SIZE));
	pcss.setInt("u_samplePattern", sampling);
	pcss.setBool("u_IsLightSrc", false);
	pcss.disable();
	
	glEnable(GL_CULL_FACE);
	genQueries(queryID_VIR);
	genQueries(queryID_lightPass);
	lastTime = SDL_GetTicks();
	
	// Render:
    while (!display.isClosed()) {
		numFrames++;
        glFinish();
        handleKeys();
    	
        Camera camera(cameraPosition, fov, (float)WIDTH, (float)HEIGHT, zNear, zFar, lookAt, bbox);
        glm::mat4 projection = camera.GetPerspProj();
        glm::mat4 view = camera.GetView();
		//glm::mat4 viewProj = camera.GetPersViewProj();
    	
        display.Clear(0.0f, 0.0f, 0.0f, 1.0f);
    	if(dTheta > 0)
			modelScene = glm::rotate(modelScene, glm::sin(dTheta), glm::vec3(0.0f, 1.0f, 0.0f)); // If dTheta is non-zero.
		glBeginQuery(GL_TIME_ELAPSED, queryID_VIR[queryBackBuffer][0]);
		glEnable(GL_DEPTH_TEST);
    	
    	/* Create ShadowMap*/
			//1. Bind m_shadowMapFBO for writing
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		BindFBOForWriting();
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			shadowMapShader.use();
			shadowMapShader.setMat4("u_viewProj", lightViewProj);
			shadowMapShader.setMat4("u_model", modelScene);
			scene.Draw(shadowMapShader);
			shadowMapShader.disable();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
			//2. Render Shadow map if pressed "p" key
		if (depthMapToggle)		
		{
			glViewport(0, 0, WIDTH, HEIGHT);
			lightViewShader.use();
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit]);
			DrawQuadGL();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			lightViewShader.disable();
		}
		else
		{
			ModelLight = glm::mat4();
			ModelLight = glm::translate(ModelLight, lightPosition);
			ModelLight = glm::scale(ModelLight, glm::vec3(LIGHT_SIZE, LIGHT_SIZE, 0.0f));	// it's a bit too big for our scene, so scale it down
			glViewport(0, 0, WIDTH, HEIGHT);
			pcss.use();
				pcss.setBool("u_IsLightSrc", false);
				pcss.setVec3("u_eye", cameraPosition);
				pcss.setMat4("u_depthBiasMVP", depthBiasMVP);
				pcss.setMat4("u_view", view);
				pcss.setVec2("u_lightRadiusUV", glm::vec2(LIGHT_SIZE));
				pcss.setMat4("u_projection", projection);
				BindFBOForReading(2);
				pcss.setInt("u_shadowMap", 2);
				pcss.setInt("u_shadowMapPcf", 3);
				pcss.setMat4("u_model", modelFloor);
				floor.Draw(pcss);
				pcss.setMat4("u_model", modelWall);
				wall .Draw(pcss);
				pcss.setMat4("u_model", modelScene);
				scene.Draw(pcss);
				pcss.setMat4("u_model", ModelLight);
				pcss.setBool("u_IsLightSrc", true);
				light.Draw(pcss);
			pcss.disable();			
		}
		glEndQuery(GL_TIME_ELAPSED);
        /*****************************************
         * Display time and number of points     *
        /*****************************************/
        glGetQueryObjectui64v(queryID_VIR[queryFrontBuffer][0], GL_QUERY_RESULT, &virTime);
        avgVIR_time += virTime / 1000000.0;
        if (numFrames % avgNumFrames == 0 && runtime) {
			std::cout << "PCSS: " << avgVIR_time / avgNumFrames;
			std::cout << "\tLIGHT SIZE: " << LIGHT_SIZE;
            std::cout << std::endl;
			avgVIR_time = 0;
        }		
        swapQueryBuffers();
        display.Update();        
    }

	// Free the buffers
	glDeleteFramebuffers(1, &m_shadowMapFBO);
	glDeleteTextures(1, &depthTexture);
	for(int i = 0; i < NumTextureUnits; i++)
	{
		glDeleteTextures(1, &m_textures[i]);
		glDeleteTextures(1, &m_samplers[i]);
	}
    return 0;
}


void initRendering()
{
	GLint depthBits;
	glGetIntegerv(GL_DEPTH_BITS, &depthBits);
	std::cout << "depth Bits: " << depthBits << std::endl;

	glGenSamplers(NumTextureUnits, m_samplers);
	for (GLuint unit = 0; unit < NumTextureUnits; ++unit)
	{
		glBindSampler(unit + 2, m_samplers[unit]);
	}
	
	auto status = initShadowMap(1);
	if (!status)
		std::cout << "Error in init shadow map" << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepthf(1.0f);

	for (GLuint unit = 2; unit < 2 + NumTextureUnits; ++unit)
	{
		glBindSampler(unit,0);
	}
}

bool initShadowMap(int unit)
{
	bool result = false;
	
	GLuint prevFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER, (GLint*)&prevFBO);

	// Setup the shadowmap depth sampler
	glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Setup the shadowmap PCF sampler
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glGenFramebuffers(1, &m_shadowMapFBO);
	if (m_shadowMapFBO == 0)
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	
	// Generate shadow map textures;
	glActiveTexture(GL_TEXTURE0 + ShadowDepthTextureUnit);				
	glGenTextures(1, &m_textures[ShadowDepthTextureUnit]);
	if (&m_textures[ShadowDepthTextureUnit] == 0)
		return false;
	m_textures[ShadowPcfTextureUnit] = m_textures[ShadowDepthTextureUnit];
	glBindTexture(GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit]);
	glTexStorage2D(	GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT);
	
	// Add the shadowmap texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit], 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
	return true;
			
}	

void BindFBOForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapFBO);
}

void BindFBOForReading(GLenum TextureUnit)
{
	for (GLuint unit = 0; unit < NumTextureUnits; ++unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit + TextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_textures[unit]);
		glBindSampler(unit + TextureUnit, m_samplers[unit]);
	}
}

void DrawQuadGL()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// call this function when initializating the OpenGL settings
void genQueries(GLuint qid[][1]) {

    glGenQueries(1, qid[queryBackBuffer]);
    glGenQueries(1, qid[queryFrontBuffer]);

    // dummy query to prevent OpenGL errors from popping out
    glQueryCounter(qid[queryFrontBuffer][0], GL_TIME_ELAPSED);
}

// aux function to keep the code simpler
void swapQueryBuffers() {

    if (queryBackBuffer) {
        queryBackBuffer = 0;
        queryFrontBuffer = 1;
    }
    else {
        queryBackBuffer = 1;
        queryFrontBuffer = 0;
    }
}
