#pragma once

#define WIDTH 1024
#define HEIGHT 1024
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
#define HEIGHT_BY_2 512
#define QUERY_BUFFERS 2
#define QUERY_COUNT 1

Display display(WIDTH, HEIGHT, "PCSS");

//VIR
enum {
	NUM_IMAGES = 1,
	LIGHT_SIZE_X = 1,
	LIGHT_SIZE_Y = 1,
	OPTIMIZATION_VAL = 1,
	UBO_IMAGE_BINDING_ID = 0,
	UBO_LIGHTS_BINDING_ID = 1,
	UBO_MVP_MATRICES_BINDING_ID = 2,
	UBO_INVERSE_MVP_BINDING_ID = 4,
	UBO_VIEW_MATRICES_BINDING_ID = 5,
	UBO_PROJECTION_MATRICES_BINDING_ID = 6
};

enum ModelName
{
	TRIANGLE = 0,
	STREET_SCENE = 1,
	ELEPHANT = 2,
	BUNNY = 3,
	LUCY_50K = 4,
	LUCY_100K = 5,
	LUCY_250K = 6,
	LUCY_500K = 7,
	LUCY_1M = 8,
	LUCY_2M = 9,
	BUDDHA = 10,
	CUBE = 11,
	GIRL = 12,
	CHESTNUT = 13,
	SPACE_SUIT = 14,
	ENVIRONMENT = 15,
	SQUARE = 16,
	CONFERENCE = 17
};

enum ShadowTechnique
{
	None = 0,
	PCSS,
	PCF
};

GLuint queryID_VIR[QUERY_BUFFERS][QUERY_COUNT];
GLuint queryID_lightPass[QUERY_BUFFERS][QUERY_COUNT];
int queryBackBuffer = 0, queryFrontBuffer = 1;

/**************Camera ********************/
bool printCameraCoord = true;
const glm::vec3 cameraDefaultPosition(5.0f, 6.0f, 8.0f);
const glm::vec3 lookAtDefault(0.0f, 0.0f, 0.0f);
glm::vec3 cameraPosition(0.60f, -01.20f, 1.30f);
glm::vec3 lookAt(-4.20f, -10.65f, -15.4f);
glm::vec3 front = glm::normalize(cameraPosition - lookAt);
glm::vec3 side = glm::normalize(cameraPosition - lookAt);
float VIRfov = glm::radians(45.0f);
float fov = glm::radians(60.0f);
float lightFOV = VIRfov;
float defaultFOV = fov;
float zNear = 0.10f;
float zFar = 40.0f;
float counter = 0.0f;
float bbox = 1.0f;
bool depthMapToggle = false;
int depthMapIndex = 0;

/**************Lights********* ***/
glm::vec3 lightPosition(-1.0f, -0.10f, 5.8f);
glm::vec3 lightLookAt(5.2f, -12.50f, -16.0f);
glm::vec3 defaultLightPosition(5.0f, 10.0f, 10.0f);
glm::vec3 Lfront = glm::normalize(lightPosition - lightLookAt);
glm::vec3 Lside = Lfront;

float dt = 0.1f;
// FPS parameters
double lastTime = 0;
int numFrames = 0;
int avgNumFrames = 628;
float LIGHT_SIZE = 0.5;
bool runtime = false; bool csv = true;
bool CountNumberOfPoints = true;
bool debug = false;


GLuint m_shadowMapFBO;
ShadowTechnique m_shadowTechnique;

GLuint depthMap;


static const GLint ShadowDepthTextureUnit = 0;
static const GLint ShadowPCFTextureUnit = 1;
static const GLint NumTextureUnits = 2;

GLuint m_samplers[NumTextureUnits];
GLuint m_textures[NumTextureUnits];

