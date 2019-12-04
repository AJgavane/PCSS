#include <iostream>
#include<vector>
#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "camera.h"
#include <GL/GL.h>
#include <GL/glext.h>
#include "Constants.h"
#include "HandelKeys.h"


void printVec(const std::string& str, const glm::vec3& v);
void printMatrix(glm::mat4 M);
void GetFrameTime();
void genQueries(GLuint qid[][1]);
void swapQueryBuffers();

bool initShadowMap(int unit);
void BindFBOForWriting();
void BindFBOForReading(GLenum TextureUnit);
void DrawQuadGL();


void initRendering();
void initTexutures();

void perspectiveFrustum(const glm::mat4& proj, float width, float height, float near, float far);

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
    float dTheta = 0.01; int rotAngle = 45;
	std::fill(&m_samplers[0], &m_samplers[NumTextureUnits], 0);
	std::fill(&m_textures[0], &m_textures[NumTextureUnits], 0);
	Model floor("./res/models/floor/floor.obj");
	printVec("Floor Center: ", floor.GetCenter());
	printVec("Floor Extent: ", floor.GetExtent());
    glm::mat4 modelFloor;
    modelFloor = glm::scale(modelFloor, glm::vec3(01.0f));	// it's a bit too big for our scene, so scale it down
    modelFloor = glm::translate(modelFloor, glm::vec3(0.00f, -1.00f, 1.00f));

	
	
    glm::mat4 modelPalm;
	Model palm;
	int model_number = ModelName::LUCY_2M;
	CountNumberOfPoints = !true; debug = !true;
	runtime = true; avgNumFrames = 125; csv = true;  dTheta = 0;
    switch (model_number)
    {
	case ModelName::BREAKFASTROOM:
		palm.LoadMeshModel("./res/models/breakfast_room/breakfast_room.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, 0.60f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(1.0f));	// for cube
		//modelPalm = glm::rotate(modelPalm, -2.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(23.6953, 15.1463, 0.946266);
		lightLookAt = glm::vec3(8.92292, 9.14627, 2.88979);
		cameraPosition =cameraDefaultPosition;
		lookAt = lookAtDefault;
		LIGHT_SIZE = 5.0f;
		break;
	case ModelName::TRIANGLE:
		// 1 triangle
		palm.LoadMeshModel("./res/models/triangle/triangle.obj");
		modelPalm = glm::scale(modelPalm, glm::vec3(0.3f));
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -00.750f, 2.0));
		modelPalm = glm::scale(modelPalm, glm::vec3(0.03f, 1.00f, .50f));
		lightPosition = glm::vec3(-0.0f, 1.0f, 4.10f);
		lightLookAt = glm::vec3(0.0f, 0.00f, -1.0f);
		cameraPosition = glm::vec3(0.820f, 0.50f, 2.30f);
		lookAt = glm::vec3(-4.03f, -9.8f, -15.76f);
		break;
	case ModelName::SQUARE:
		// 1 triangle
		palm.LoadMeshModel("./res/models/triangle/square.obj");
		modelPalm = glm::scale(modelPalm, glm::vec3(0.3f));
		modelPalm = glm::translate(modelPalm, glm::vec3(-1.00f, -1.00f, -3.0));
		lightPosition = glm::vec3(0.0f, 5.0f, 10.0f);
		lightLookAt = glm::vec3(0.0f, 0.00f, -1.0f);
		cameraPosition = cameraDefaultPosition; // glm::vec3(0.820f, 0.50f, 2.30f);
		lookAt = lookAtDefault;// glm::vec3(-4.03f, -9.8f, -15.76f);
		//dTheta = 0;
		break;
	case ModelName::STREET_SCENE :
		palm.LoadMeshModel("./res/models/streetscene/streetscene.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.750f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(0.015f));	// for cube
		modelPalm = glm::rotate(modelPalm, -2.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-4.73f, 1.49f, 5.0f);
		lightLookAt = glm::vec3(6.28f, -7.0f, -10.40f);
		cameraPosition =  glm::vec3(-1.269, 1.155, 3.553);
		lookAt =  glm::vec3(4.501, -4.845, -11.934);
		break;
	case ModelName::ELEPHANT :
		palm.LoadMeshModel("./res/models/elephant/elephant.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.750f, 0.0)); //for cube
		bbox = 0.01;
		modelPalm = glm::scale(modelPalm, glm::vec3(0.0015f));	// for cube
		lightPosition = glm::vec3(-4.73f, 5.49f, 10.0f);
		lightLookAt = glm::vec3(6.28f, -7.0f, -12.40f);
		cameraPosition = glm::vec3(0.33f, 1.48f, 2.50f);
		lookAt = glm::vec3(0.6f, -4.54f, -5.3f);
		break;
	case ModelName::BUNNY :
		palm.LoadMeshModel("./res/models/bunny/bunny.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.850f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(0.5f));	// for cube
		lightPosition = glm::vec3(-4.73f, 5.49f, 10.0f);
		lightLookAt = glm::vec3(6.28f, -7.0f, -12.40f);
		cameraPosition = glm::vec3(1.51f, 1.30f, 2.0f);
		lookAt = glm::vec3(-2.1f, -4.7f, -3.5f);
		break;
	case ModelName::ENVIRONMENT :
		palm.LoadMeshModel("./res/models/environment/environment.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.700f, -0.00f, -2.0));
		modelPalm = glm::scale(modelPalm, glm::vec3(0.0011f));
		modelPalm = glm::rotate(modelPalm, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-3.73f, 3.49f, 5.0f);
		lightLookAt = glm::vec3(6.28f, -7.0f, -12.40f);
		cameraPosition = glm::vec3(0.67f, 1.90f, 0.0f);
		lookAt = glm::vec3(3.1f, -10.7f, -15.5f);
		break;
	case ModelName::LUCY_50K:
		palm.LoadMeshModel("./res/models/Alucy/50k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		//lightPosition = glm::vec3(0.70f, 0.70f, 2.00f);
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		bbox = 0.75f;
		break;
	case ModelName::LUCY_100K:
		palm.LoadMeshModel("./res/models/Alucy/100k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(0.0f, 1.0f, 2.0f);
		//lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		lightLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		bbox = 0.75f;
		break;
	case ModelName::LUCY_250K:
		palm.LoadMeshModel("./res/models/Alucy/250k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		bbox = 0.75f;
		break;
	case ModelName::LUCY_500K:
		palm.LoadMeshModel("./res/models/Alucy/500k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		break;
	case ModelName::LUCY_1M:
		palm.LoadMeshModel("./res/models/Alucy/1m.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		bbox = 0.75f;
		break;
	case ModelName::LUCY_2M:
		palm.LoadMeshModel("./res/models/Alucy/2m.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		bbox = 0.75f;
		break;
	case ModelName::BUDDHA:
		palm.LoadMeshModel("./res/models/buddha/buddha.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(-0.50f, -0.63f, -0.70f)); 
		modelPalm = glm::scale(modelPalm, glm::vec3(0.40f));	
		modelPalm = glm::rotate(modelPalm, -2.90f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.250f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.042, -0.054, -0.218);
		lookAt = glm::vec3(-5.275, -6.054, -8.251);;
		break;
	case ModelName::CUBE:
		palm.LoadMeshModel("./res/models/cube/cube.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -00.400f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(0.05f));	// for cube
		lightPosition = glm::vec3(-0.0f, 1.0f, 4.10f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.820f, 0.50f, 1.30f);
		lookAt = glm::vec3(-4.03f, -9.8f, -15.76f);
		break;
	case ModelName::GIRL:
		palm.LoadMeshModel("./res/models/girl/girl.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, 0.0f, 0.50));
		modelPalm = glm::scale(modelPalm, glm::vec3(0.02));	// for spacesuit
		modelPalm = glm::rotate(modelPalm, -0.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // for sapce suit
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.510f, 2.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		break;
	case ModelName::CHESTNUT:
		palm.LoadMeshModel("./res/models/chestnut/chestnut.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(-0.50f, -0.63f, -0.70f)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.001f));	// for lucy
		modelPalm = glm::rotate(modelPalm, -1.507f, glm::vec3(1.0f, 0.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.250f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.35, 0.5, 0.03);
		lookAt = glm::vec3(-3.0, -4.9, -5.550);
		break;
	case ModelName::SPACE_SUIT:
		palm.LoadMeshModel("./res/models/spacesuit/spacesuit.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.80f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(0.8f));	// for cube
		modelPalm = glm::rotate(modelPalm, -2.80f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-4.73f, 5.49f, 5.0f);
		lightLookAt = glm::vec3(6.28f, -7.0f, -12.40f);
		cameraPosition = glm::vec3(0.13f, 2.0f, -1.830f);
		lookAt = glm::vec3(-0.50f, -4.f, -7.f);
		break;
	default:
		palm.LoadMeshModel("./res/models/Alucy/50k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		break;
    }

	printVec("Model Center: ", modelPalm * glm::vec4(palm.GetCenter(),1.0));
	printVec("Model Extent: ", modelPalm * glm::vec4(palm.GetExtent(),1.0));
	initRendering();
	Shader basicShader("./res/basic.vs", "./res/basic.fs");
	basicShader.use(); basicShader.disable();

	// Light constants
	//Camera altCamera(lightPosition, lightFOV, (float)SHADOW_WIDTH, (float)SHADOW_HEIGHT, 1.0f, zFar, lightLookAt, bbox);

	
	glm::mat4 lightView =glm::lookAt(lightPosition, lightLookAt, glm::vec3(0.0f, 1.0f, 0.0f));  //altCamera.GetView();
	glm::vec3 center = palm.GetCenter();
	glm::vec3 extents = palm.GetExtent();
	glm::vec3 box[2];
	box[0] = center - extents;
	box[1] = center + extents;
	glm::vec3 bboxScaled[2];
	transformBoundingBox(bboxScaled, box, lightView * modelPalm);
	
	FrustumWidth = std::max(fabs(bboxScaled[0][0]), fabs(bboxScaled[1][0])) * 2.0f;
	FrustumHeight = FrustumWidth; // std::max(fabs(bboxScaled[0][1]), fabs(bboxScaled[1][1])) * 2.0f;//*/
	l_zNear = -bboxScaled[1][2];
	//LIGHT_SIZE /= FrustumWidth;
	std::cout << "FW/FH: " << FrustumWidth << std::endl;
	std::cout << "l_zNear: " << l_zNear << std::endl;
	std::cout << "LIGHT_SIZE: " << LIGHT_SIZE << std::endl;
	bbox = 1.0;
	glm::mat4 lightProj = glm::perspective(lightFOV, 1.0f, 1.0f, zFar);
	//glm::mat4 lightProj = glm::frustum(-FrustumWidth/2, FrustumWidth/2, -FrustumHeight/2, FrustumHeight/2, 2.0f, zFar);
	glm::mat4 lightViewProj = lightProj * lightView;
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	glm::mat4 depthBiasMVP = biasMatrix * lightViewProj;
	glm::mat4 invLightView = glm::inverse(lightView);
//	lightPosition = transformCoord(invLightView, lightPosition);
	printVec("....Light Position: ", lightPosition);

	glm::mat4 ModelLight;
	Model light("./res/models/square/square.obj");
	ModelLight = glm::scale(ModelLight, glm::vec3(LIGHT_SIZE, LIGHT_SIZE, 1.0f));	// it's a bit too big for our scene, so scale it down
	ModelLight = glm::translate(ModelLight, lightPosition);
	//printVec("light Center: ", ModelLight * glm::vec4(light.GetCenter(), 1.0));
	//printVec("light Extent: ", ModelLight * glm::vec4(light.GetExtent(), 1.0));

	
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

	Shader depthPrePass("./res/depthPrePass.vs", "./res/depthPrePass.fs");
	depthPrePass.use(); depthPrePass.disable();
	
	Shader hardShadows("./res/hardShadows.vs", "./res/hardShadows.fs");
	hardShadows.use();
	hardShadows.setMat4("u_depthBiasMVP", depthBiasMVP);
	hardShadows.disable();

	Shader pcss("./res/pcss.vs", "./res/pcss.fs");
	pcss.use();
	pcss.setMat4("u_depthBiasMVP", depthBiasMVP);
	pcss.setVec3("u_lightPosition", lightPosition);
	pcss.setMat4("u_lightView", lightView);
	pcss.setFloat("u_light_zNear", l_zNear);
	pcss.setFloat("u_light_zFar", zFar);
	pcss.setVec2("u_lightRadiusUV", glm::vec2(LIGHT_SIZE));
	pcss.setInt("u_samplePattern", SamplePattern::POISSON_64_128);
	pcss.setBool("u_IsLightSrc", false);
	pcss.disable();
	
	glDepthFunc(GL_LESS); glEnable(GL_CULL_FACE);
	genQueries(queryID_VIR);
	genQueries(queryID_lightPass);
	lastTime = SDL_GetTicks();
	// Render:
    while (!display.isClosed()) {
		numFrames++;
        glFinish();
        handleKeys();
        Camera camera(cameraPosition, fov, (float)WIDTH, (float)HEIGHT, zNear, zFar, lookAt, bbox);
		glm::mat4 viewProj = camera.GetPersViewProj();
        glm::mat4 projection = camera.GetPerspProj();
        glm::mat4 view = camera.GetView();
        display.Clear(0.0f, 0.0f, 0.0f, 1.0f);
        modelPalm = glm::rotate(modelPalm, glm::sin(dTheta), glm::vec3(0.0f, 1.0f, 0.0f)); // for sapce suit
   	
		glEnable(GL_DEPTH_TEST);
    	// Create ShadowMap
		// Bind FBO for writing
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		BindFBOForWriting();
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			shadowMapShader.use();
			shadowMapShader.setMat4("u_viewProj", lightViewProj);
			shadowMapShader.setMat4("u_model", modelPalm);
			palm.Draw(shadowMapShader);
			shadowMapShader.disable();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		// Render Shadow map
		if (depthMapToggle)		
		{
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
			glBeginQuery(GL_TIME_ELAPSED, queryID_VIR[queryBackBuffer][0]);
			// Depth Prepass
			/*depthPrePass.use();
				depthPrePass.setMat4("u_projection", projection);
				depthPrePass.setMat4("u_view", view);
				depthPrePass.setMat4("u_model", modelPalm);
				palm.Draw(depthPrePass);
				depthPrePass.setMat4("u_model", modelFloor);
				floor.Draw(depthPrePass);
			depthPrePass.disable();//*/
			//glClear(GL_COLOR_BUFFER_BIT);
			if (!true)
			{
				//glDepthFunc(GL_EQUAL);
				glViewport(0, 0, WIDTH, HEIGHT);
				hardShadows.use();
					hardShadows.setVec3("u_eye", cameraPosition);
					hardShadows.setVec3("u_lightPosition", lightPosition);
					hardShadows.setMat4("u_depthBiasMVP", depthBiasMVP);
					hardShadows.setMat4("u_projection", projection);
					hardShadows.setMat4("u_view", view);
					BindFBOForReading(2);
					hardShadows.setInt("u_shadowMap", 2);
					hardShadows.setMat4("u_model", modelPalm);
					palm.Draw(hardShadows);
					hardShadows.setMat4("u_model", modelFloor);
					floor.Draw(hardShadows);
				hardShadows.disable();
				glDepthFunc(GL_LEQUAL);//*/
			}
			else 
			{
				//glDepthFunc(GL_EQUAL);
				glViewport(0, 0, WIDTH, HEIGHT);
				pcss.use();
					pcss.setBool("u_IsLightSrc", false);
					pcss.setVec3("u_eye", cameraPosition);
					pcss.setMat4("u_depthBiasMVP", depthBiasMVP);
					pcss.setMat4("u_view", view);
					pcss.setMat4("u_projection", projection);
					BindFBOForReading(2);
					pcss.setInt("u_shadowMap", 2);
					pcss.setInt("u_shadowMapPcf", 3);
					pcss.setMat4("u_model", modelPalm);
					palm.Draw(pcss);
					pcss.setMat4("u_model", modelFloor);
					floor.Draw(pcss);
					pcss.setMat4("u_model", ModelLight);
					pcss.setBool("u_IsLightSrc", true);
					light.Draw(pcss);
				pcss.disable();
				glDepthFunc(GL_LEQUAL);//*/
			}
			glEndQuery(GL_TIME_ELAPSED);
		}
        /*****************************************
         * Display time and number of points     *
        /*****************************************/
        glGetQueryObjectui64v(queryID_VIR[queryFrontBuffer][0], GL_QUERY_RESULT, &virTime);
        avgVIR_time += virTime / 1000000.0;
        if (numFrames % avgNumFrames == 0 && runtime) {
            std::cout << "VIR: " << avgVIR_time / avgNumFrames;
            std::cout << std::endl;
			avgVIR_time = 0;
        }		
        swapQueryBuffers();
        display.Update();        
    }

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
	for (GLuint unit = 2; unit < 2 + NumTextureUnits; ++unit)
	{
		glBindSampler(unit, m_samplers[unit]);
	}
	
	auto status = initShadowMap(1);
	if (!status)
		std::cout << "Error in init shadow map" << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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
	
	switch (unit)
	{
		case 0:
			{
				GLuint prevFBO = 0;
				glGetIntegerv(GL_FRAMEBUFFER, (GLint*)&prevFBO);
				// Create the FBO
				glGenFramebuffers(1, &m_shadowMapFBO);
				// Create the depth buffer
				glGenTextures(1, &depthTexture);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

				
				glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

				// Disable writes to the color buffer
				glDrawBuffer(GL_NONE);
				//glReadBuffer(GL_NONE);

				GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (Status != GL_FRAMEBUFFER_COMPLETE) {
					printf("FB error, status: 0x%x\n", Status);
					return false;
				}
				glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
				return true;
			}
		case 1:
			{
				GLuint prevFBO = 0;
				glGetIntegerv(GL_FRAMEBUFFER, (GLint*)&prevFBO);

				// Setup the shadowmap depth sampler
				glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				//glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				//glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				//glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				//glSamplerParameteri(m_samplers[ShadowDepthTextureUnit], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

				// Setup the shadowmap PCF sampler
				glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glSamplerParameteri(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				//glSamplerParameterfv(m_samplers[ShadowPcfTextureUnit], GL_TEXTURE_BORDER_COLOR, borderColor);
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
				/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//*/
				/*glBindTexture(GL_TEXTURE_2D, 0);
				glBindTexture(GL_TEXTURE_2D, m_textures[ShadowPcfTextureUnit]);
				glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT);
				/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				//*/
				// Add the shadowmap texture to the framebuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit], 0);
				glDrawBuffer(GL_NONE);
				//glReadBuffer(GL_NONE);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					return false;

				glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
				return true;
			}
		default:
			{
				return false;
			}
	}
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


void printVec(const std::string& str, const glm::vec3& v)
{
    std::cout << str << " {" << v[0] << ", " << v[1] << ", " << v[2] << "}" << std::endl;
}

void printMatrix(glm::mat4 M) {
    std::cout << std::setprecision(3) << std::fixed;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << M[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}