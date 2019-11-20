#include <iostream>
#include<vector>
#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "transform.h"
#include "camera.h"
#include <GL/GL.h>
#include <GL/glext.h>
#include "Constants.h"


void handleKeys();
void mouse_callback(double xpos, double ypos);
void MakeSomeTransformations(Transform &t1, Transform &t2, Transform &t3);
void printVec(const std::string& str, const glm::vec3& v);
void printMatrix(glm::mat4 M);
void GetFrameTime();
void genQueries(GLuint qid[][1]);
void swapQueryBuffers();

int main(int args, char** argv)
{

	printVec("LightOrigin: ", lightPosition);

    printVec("camera  ", cameraPosition);
    printVec("Center ", lookAt);
    float dTheta = 0.01; int rotAngle = 45;

	Shader basic("./res/basic.vs", "./res/basic.fs");
    Shader firstPass("./res/firstPass.vs", "./res/firstPass.fs", "./res/firstPass.gs");
    Shader secondPass("./res/secondPass.vs", "./res/secondPass.fs");
    Shader debugDepthQuad("./res/depthMap.vs", "./res/depthMap.fs");

    // Shader test("./res/test.vs", "./res/test.fs");
    Model floor("./res/models/floor/floor.obj");
    glm::mat4 modelFloor;
    modelFloor = glm::scale(modelFloor, glm::vec3(01.0f));	// it's a bit too big for our scene, so scale it down
    modelFloor = glm::translate(modelFloor, glm::vec3(0.00f, -1.00f, 1.00f));

    //Model cube2("./res/models/room/room.obj");
    glm::mat4 modelPalm;
    // Model palm("./res/models/palm/palm.obj");
    // modelPalm = glm::scale(modelPalm, glm::vec3(0.3f));
	Model palm;
	int model_number = ModelName::CONFERENCE;
	CountNumberOfPoints = !true; debug = !true;
	runtime = true; avgNumFrames = 100; csv = true;  dTheta = 0;
    switch (model_number)
    {
	case ModelName::CONFERENCE:
		palm.LoadMeshModel("./res/models/breakfast_room/breakfast_room.obj");
		modelPalm = glm::translate(modelPalm, glm::vec3(0.00f, -0.750f, 0.0)); //for cube
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
	    
    float theta = 0;

   
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
		glEnable(GL_CULL_FACE);

        glBeginQuery(GL_TIME_ELAPSED, queryID_VIR[queryBackBuffer][0]);
		basic.use();
		basic.setMat4("projection", projection);
		basic.setMat4("view", view);
		basic.setMat4("model", modelPalm);
		basic.setVec3("lightPosition", lightPosition);
		basic.setVec3("eye", cameraPosition);
		palm.Draw(basic);
        glEndQuery(GL_TIME_ELAPSED);

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

void handleKeys() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            /* Look for a keypress */
        case SDL_KEYDOWN:
            /* Check the SDLKey values and move change the coords */
            switch (event.key.keysym.sym) {
            case SDLK_w:
                front = glm::normalize(lookAt - cameraPosition) * dt;
                cameraPosition = cameraPosition + front;
                lookAt = lookAt + front;
                break;
            case SDLK_s:
                front = glm::normalize(lookAt - cameraPosition) * dt;
                cameraPosition = cameraPosition - front;
                lookAt = lookAt - front;
                break;
            case SDLK_a:
                side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))) * dt;
                cameraPosition = cameraPosition - side;
                lookAt = lookAt - side;
                break;
            case SDLK_d:
                side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))) * dt;
                cameraPosition = cameraPosition + side;
                lookAt = lookAt + side;
                break;
            case SDLK_q:
                cameraPosition = cameraPosition + glm::vec3(0, dt, 0);
                lookAt = lookAt + glm::vec3(0, dt, 0);
                break;
            case SDLK_e:
                cameraPosition = cameraPosition - glm::vec3(0, dt, 0);
                lookAt = lookAt - glm::vec3(0, dt, 0);
                break;
            case SDLK_p:
                depthMapToggle = !depthMapToggle;
                if (depthMapToggle) {
                    depthMapIndex += 1;
                    depthMapIndex = depthMapIndex % NUM_IMAGES;
                    std::cout << "DepthMap: " << depthMapIndex << std::endl;
                }
                break;
            case SDLK_i:
                lookAt = lookAt + glm::vec3(0, 0, dt);
                break;
            case SDLK_k:
                lookAt = lookAt + glm::vec3(0, 0, -dt);
                break;
            case SDLK_j:
                lookAt = lookAt + glm::vec3(dt, 0, 0);
                break;
            case SDLK_l:
                lookAt = lookAt + glm::vec3(-dt, 0, 0);
                break;
            case SDLK_o:
                lightPosition = lightPosition + glm::vec3(0.0, dt, 0.0);
                lightLookAt = lightLookAt + glm::vec3(0.0, dt, 0.0);
                break;
            case SDLK_u:
                lightPosition = lightPosition - glm::vec3(0.0, dt, 0.0);
                lightLookAt = lightLookAt - glm::vec3(0.0, dt, 0.0);
                break;
            case SDLK_ESCAPE:
                display.Terminate();
                std::cout << "escape\n";
                break;
            case SDLK_z:
                bbox -= dt;
                fov -= dt;
                break;
            case SDLK_x:
                bbox += dt;
                fov += dt;
                break;
            case SDLK_r:
                cameraPosition = cameraDefaultPosition;
                lookAt = lookAtDefault;
                //lightPosition = defaultLightPosition;
                //lightLookAt = lookAtDefault;
                fov = defaultFOV;
                break;
            case SDLK_c:
                printCameraCoord = true;
                runtime = !runtime;
                break;
			case SDLK_h:
				CountNumberOfPoints = !CountNumberOfPoints;
				break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            /* If the mouse is moving to the left */
            if (event.motion.xrel < 0) {
                front = glm::normalize(lookAt - cameraPosition);
                side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
                lookAt = lookAt - side;
            }
            /* If the mouse is moving to the right */
            else if (event.motion.xrel > 0) {
                front = glm::normalize(lookAt - cameraPosition);
                side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
                lookAt = lookAt + side;
            }
            break;
        default:
            break;
        }
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