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

bool initShadowMap();
void BindFBOForWriting();
void BindFBOForReading(GLenum TextureUnit);
void DrawQuadGL();


int main(int args, char** argv)
{

	printVec("LightOrigin: ", lightPosition);

    printVec("camera  ", cameraPosition);
    printVec("Center ", lookAt);
    float dTheta = 0.01; int rotAngle = 45;

	Shader basicShader("./res/basic.vs", "./res/basic.fs");
	Shader depthPassShader("./res/depthPass.vs", "./res/depthPass.fs");
	Shader lightViewShader("./res/lightView.vs", "./res/lightView.fs");

	Model floor("./res/models/floor/floor.obj");
    glm::mat4 modelFloor;
    modelFloor = glm::scale(modelFloor, glm::vec3(01.0f));	// it's a bit too big for our scene, so scale it down
    modelFloor = glm::translate(modelFloor, glm::vec3(0.00f, -1.00f, 1.00f));

    //Model cube2("./res/models/room/room.obj");
    glm::mat4 modelPalm;
    // Model palm("./res/models/palm/palm.obj");
    // modelPalm = glm::scale(modelPalm, glm::vec3(0.3f));
	Model palm;
	int model_number = ModelName::ENVIRONMENT;
	CountNumberOfPoints = !true; debug = !true;
	runtime = true; avgNumFrames = 100; csv = true;  dTheta = 0;
    switch (model_number)
    {
	case ModelName::CONFERENCE:
		palm.LoadMeshModel("./res/models/breakfast_room/breakfast_room.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, 0.60f, 0.0)); //for cube
		modelPalm = glm::scale(modelPalm, glm::vec3(1.0f));	// for cube
		//modelPalm = glm::rotate(modelPalm, -2.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(0.986308, 3.36432, -1.88091);
		lightLookAt = glm::vec3(0.986308, 1.36432, -1.88091);
		cameraPosition =cameraDefaultPosition;
		lookAt = lookAtDefault;
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
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		break;
	case ModelName::LUCY_100K:
		palm.LoadMeshModel("./res/models/Alucy/100k.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
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
		break;
	case ModelName::LUCY_2M:
		palm.LoadMeshModel("./res/models/Alucy/2m.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.810f, -2.00)); //for lucy
		modelPalm = glm::scale(modelPalm, glm::vec3(0.00075f));	// for lucy
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosition = glm::vec3(-0.0f, 0.70f, 2.50f);
		lightLookAt = glm::vec3(-2.2f, -5.10f, -13.70f);
		cameraPosition = glm::vec3(0.530f, 0.210f, -1.0f);
		lookAt = glm::vec3(-4.3f, -5.9f, -11.3f);
		break;
	case ModelName::BUDDHA:
		palm.LoadMeshModel("./res/models/buddha/buddha.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(-0.50f, -0.63f, -0.70f)); 
		modelPalm = glm::scale(modelPalm, glm::vec3(0.40f));	
		modelPalm = glm::rotate(modelPalm, 1.30f, glm::vec3(0.0f, 1.0f, 0.0f));
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

	lightViewShader.setInt("u_shadowMap", 0);
	initShadowMap();

	
    GLuint64 virTime;
    float avgVIR_time = 0.0f;
    genQueries(queryID_VIR);
    genQueries(queryID_lightPass);
    lastTime = SDL_GetTicks();
    // Test
    while (!display.isClosed()) {
        if (printCameraCoord) {
            printVec("camera  ", cameraPosition);
            printVec("Center ", lookAt);
            printCameraCoord = !printCameraCoord;

        }
        glFinish();
        handleKeys();
        Camera camera(cameraPosition, fov, (float)WIDTH, (float)HEIGHT, zNear, zFar, lookAt, bbox);
        glm::mat4 projection = camera.GetPerspProj();
        glm::mat4 view = camera.GetView();
       
       
        display.Clear(0.0f, 0.15f, 0.3f, 1.0f);
      //  theta = theta + dTheta;
      //  modelPalm = glm::rotate(modelPalm, glm::sin(dTheta), glm::vec3(0.0f, 1.0f, 0.0f)); // for sapce suit

		glEnable(GL_DEPTH_TEST);
		

		if (!depthMapToggle)
		{
			// Bind FBO for writing
			GLuint prevFBO = 0;
			glGetIntegerv(GL_FRAMEBUFFER, (GLint*)&prevFBO);
			BindFBOForWriting();
			glViewport(0, 0, WIDTH, HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable((GL_POLYGON_OFFSET_FILL));
			glPolygonOffset(4.0f, 32.0f);
			depthPassShader.use();
			depthPassShader.setMat4("projection", projection);
			depthPassShader.setMat4("view", view);
			depthPassShader.setMat4("model", modelPalm);
			palm.Draw(depthPassShader);
			depthPassShader.disable();
			glDisable(GL_POLYGON_OFFSET_FILL);
			glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
			glViewport(0, 0, WIDTH, HEIGHT);
		}

		if (!depthMapToggle)
		{
			glViewport(0, 0, WIDTH, HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			lightViewShader.use();
			lightViewShader.setInt("u_shadowMap", GL_TEXTURE0);
			BindFBOForReading(GL_TEXTURE0);
			lightViewShader.setFloat("light_zFar", zFar);
			lightViewShader.setFloat("light_zNear", zNear);
			DrawQuadGL();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			lightViewShader.disable();
		}
		else
		{
			glViewport(0, 0, WIDTH, HEIGHT);
			glBeginQuery(GL_TIME_ELAPSED, queryID_VIR[queryBackBuffer][0]);
			basicShader.use();
			basicShader.setMat4("projection", projection);
			basicShader.setMat4("view", view);
			basicShader.setVec3("lightPosition", lightPosition);
			basicShader.setVec3("eye", cameraPosition);
			basicShader.setMat4("model", modelPalm);
			palm.Draw(basicShader);
			basicShader.setMat4("model", modelFloor	);
			floor.Draw(basicShader);
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
        numFrames++;
    }
   
    return 0;
}

bool initShadowMap()
{
	GLuint prevFBO = 0;
	glGetIntegerv(GL_FRAMEBUFFER, (GLint*)&prevFBO);
	// Create the FBO
	glGenFramebuffers(1, &m_shadowMapFBO);

	// Create the depth buffer
	glGenTextures(1, &m_textures[ShadowDepthTextureUnit]);
	glBindTexture(GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit], 0);

	// Disable writes to the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
	return true;
}

void BindFBOForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapFBO);
}
void BindFBOForReading(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_textures[ShadowDepthTextureUnit]);
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