//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>
#include "Skybox.hpp"

int glWindowWidth = 1000;
int glWindowHeight = 700;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;
glm::vec3 pointLightColor;
GLuint pointLightColorLoc;


gps::Camera myCamera(glm::vec3(25.4458f, 21.9242f, 78.2126f),
					 glm::vec3(0.0f, 0.0f, -10.0f),
					 glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.35f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D scena;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

//pt animatie caine
gps::Model3D caine;
glm::vec3 caine_punct(0.0f, 0.0f, 0.0f);
float unghiCaine = 0.0f;
float viteza = 25.0f;
float delta = 0.0f;


//skybox
gps::SkyBox mySkyBox;

// pt ceata
GLint foginitLoc;
int foginit = 0;
GLfloat fogDensity;
GLuint fogDensityLoc;

//pt noapte
GLint isNightLoc;
GLint nightLightColorLoc;
glm::vec3 nightLightColor = glm::vec3(0.05f, 0.05f, 0.05f);
int night = 0;
bool showDepthMap;

//pt felinare
glm::vec3 lightPos1;
GLuint lightPos1Loc;

glm::vec3 lightPos2;
GLuint lightPos2Loc;

glm::vec3 lightPos3;
GLuint lightPos3Loc;

glm::vec3 lightPos4;
GLuint lightPos4Loc;

glm::vec3 lightPos5;
GLuint lightPos5Loc;

glm::vec3 lightPos6;
GLuint lightPos6Loc;

glm::vec3 lightPos7;
GLuint lightPos7Loc;

GLint lightONLoc;
int lightON = 0;

//apt animatie
float rotateCamera = 0;
glm::mat4 originalView; 

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

	bool mouse = true;
	float lastX = 400.0f;
	float lastY = 300.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;

	void mouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS)
			return;

			if (mouse)
			{
				mouse = false;
				lastX = xpos;
				lastY = ypos;
				return;
			}

			float x_offset = xpos - lastX;
			float y_offset = lastY - ypos;

			lastX = xpos;
			lastY = ypos;

			float sensitivity = 0.1f;
			x_offset *= sensitivity;
			y_offset *= sensitivity;

			yaw += x_offset;
			pitch += y_offset;


			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			myCamera.rotate(pitch, yaw);
			view = myCamera.getViewMatrix();
			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

float fov = 45.0f;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 80.0f)
			fov = 80.0f;

		myCustomShader.useShaderProgram();
		projection = glm::perspective(glm::radians(fov), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}


void processMovement()
{
	static float pitch = 0.0f;
	static float yaw = -90.0f;
	if (pressedKeys[GLFW_KEY_Q]) {
		yaw -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		yaw += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_U]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_I]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}
	 myCamera.rotate(pitch, yaw);

	 if (pressedKeys[GLFW_KEY_F])
	 {
		 fogDensity += 0.002f;
		 if (fogDensity >= 0.3f)
			 fogDensity = 0.3f;
		 myCustomShader.useShaderProgram();
		 glUniform1f(fogDensityLoc, fogDensity);
	 }

	 if (pressedKeys[GLFW_KEY_G])
	 {
		 fogDensity -= 0.002f;
		 if (fogDensity <= 0.0f)
			 fogDensity = 0.0f;
		 myCustomShader.useShaderProgram();
		 glUniform1f(fogDensityLoc, fogDensity);

	 }

	 if (pressedKeys[GLFW_KEY_N]) {
		 night = !night;  
		 myCustomShader.useShaderProgram();

		 if (night == 1) {
			 glUniform3fv(lightColorLoc, 1, glm::value_ptr(nightLightColor));
		 }
		 else {
			 glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
		 }

		 glUniform1i(isNightLoc, night);
		 pressedKeys[GLFW_KEY_N] = false;  
	 }

	if (pressedKeys[GLFW_KEY_1]) {
		lightON = !lightON;  
		myCustomShader.useShaderProgram();
		glUniform1i(lightONLoc, lightON);
		pressedKeys[GLFW_KEY_1] = false;  
	}

	if (pressedKeys[GLFW_KEY_K])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_P])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_O])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_L])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_SMOOTH);
	}

	if (pressedKeys[GLFW_KEY_B])
	{
		if (rotateCamera == 0)
		{
			rotateCamera = 1;
			originalView = view;
		}
		else
		{
			rotateCamera = 0;
			view = myCamera.getViewMatrix();
		}
		pressedKeys[GLFW_KEY_B] = false;  
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetScrollCallback(glWindow, scroll_callback);

	//glfwSetCursorPosCallback(glWindow, mouseCallback);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION); 
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS); 
	//glEnable(GL_CULL_FACE); // cull face
	//glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); 

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	scena.LoadModel("objects/adina/adina.obj");
	caine.LoadModel("objects/caine/caine.obj");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
}



glm::mat4 computeLightSpaceTrMatrix() {
	// Definim un punct fix către care se uită lumina
	glm::vec3 lightTarget(0.0f, 0.0f, 0.0f);

	// Poziția luminii să fie independentă de cameră
	glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

	// Calculăm view matrix-ul pentru lumină
	glm::mat4 lightView = glm::lookAt(lightPos,
		lightTarget,
		glm::vec3(0.0f, 1.0f, 0.0f));

	// Definim dimensiunile volumului de proiecție ortografică
	float near_plane = 1.0f, far_plane = 10.0f;
	float ortho_size = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-ortho_size, ortho_size,
		-ortho_size, ortho_size,
		near_plane, far_plane);

	return lightProjection * lightView;
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor");
	glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
	//lampa
	lightPos1 = glm::vec3(-13.3801f, 9.07357f, 12.1422f);
	lightPos1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos1");
	glUniform3fv(lightPos1Loc, 1, glm::value_ptr(lightPos1));
	//lampa2
	lightPos7 = glm::vec3(11.312f, 7.71f, -14.067f);
	lightPos7Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos7");
	glUniform3fv(lightPos7Loc, 1, glm::value_ptr(lightPos7));
	//felinare
	lightPos2 = glm::vec3(18.7837f, 11.4421f, -21.2394f);
	lightPos2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos2");
	glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));

	lightPos3 = glm::vec3(-34.9232f, 11.2771f, 54.914f);
	lightPos3Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos3");
	glUniform3fv(lightPos3Loc, 1, glm::value_ptr(lightPos3));

	lightPos4 = glm::vec3(35.1458f, 11.0837f, 73.6979f);
	lightPos4Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos4");
	glUniform3fv(lightPos4Loc, 1, glm::value_ptr(lightPos4));

	lightPos5 = glm::vec3(-75.6762f, 11.4327f, -101.583f);
	lightPos5Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos5");
	glUniform3fv(lightPos5Loc, 1, glm::value_ptr(lightPos5));

	lightPos6 = glm::vec3(-94.6991f, 11.0917f, -15.4538f);
	lightPos6Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos6");
	glUniform3fv(lightPos6Loc, 1, glm::value_ptr(lightPos6));
	
	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(depthMapShader.shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model)); 

	nightLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "nightColor");
	glUniform3fv(nightLightColorLoc, 1, glm::value_ptr(nightLightColor));

	isNightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isNight");
	glUniform1i(isNightLoc, night);

	fogDensity = 0.0f;
	fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc,fogDensity);

	lightONLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightON");
	glUniform1i(lightONLoc, lightON);

}


void initFBO() {
	// Mărește rezoluția shadow map-ului pentru umbre mai clare
	const unsigned int SHADOW_WIDTH = 4096;  // sau 2048 în funcție de performanță
	const unsigned int SHADOW_HEIGHT = 4096;

	glGenFramebuffers(1, &shadowMapFBO);

	// Create depth texture
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Îmbunătățește filtrarea texturii
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void  initSkybox() {
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/dreapta.png");
	faces.push_back("skybox/stanga.png");
	faces.push_back("skybox/sus.png");
	faces.push_back("skybox/jos.png");
	faces.push_back("skybox/inFata.png");
	faces.push_back("skybox/inSpate.png");
	mySkyBox.Load(faces);
}

void drawObjects(gps::Shader shader, bool depthPass) 
{
	shader.useShaderProgram();

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	model = glm::mat4(1.0f); 
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	GLint modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	GLint projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");

	if (modelLoc != -1)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	if (viewLoc != -1)
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	if (projectionLoc != -1)
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	scena.Draw(myCustomShader);
	mySkyBox.Draw(skyboxShader, view, projection);
	glDisable(GL_BLEND);
}

void renderCaine(gps::Shader shader) {

	unghiCaine += viteza * delta; 

	glm::mat4 caineModel = glm::mat4(1.0f); 
	caineModel = glm::translate(caineModel, caine_punct);
	caineModel = glm::rotate(caineModel, glm::radians(unghiCaine), glm::vec3(0.0f, 1.0f, 0.0f));
	caineModel = glm::translate(caineModel, -caine_punct);
	caineModel = glm::scale(caineModel, glm::vec3(0.5f));

	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(caineModel));

	caine.Draw(myCustomShader);
}

void updateCameraRotation() {
	if (rotateCamera) {
		float cameraY = 100.0f;
		float radius = 100.0f;
		float cameraX = sin(glfwGetTime()) * radius;
		float cameraZ = cos(glfwGetTime()) * radius;

		glm::vec3 cameraPosition = glm::vec3(cameraX, cameraY, cameraZ);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
	}
	else {
		view = myCamera.getViewMatrix();
	}

	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void initShadowMap() {
	glGenFramebuffers(1, &shadowMapFBO);

	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE); // Nu avem nevoie de color buffer
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene() {
	
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);  // Reduce shadow artifacts

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	drawObjects(depthMapShader, true);

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Render scene normally with shadows
	glViewport(0, 0, retina_width, retina_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myCustomShader.useShaderProgram();

		///view = myCamera.getViewMatrix();
		///glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		if (!rotateCamera) {
			view = myCamera.getViewMatrix();
		}
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		lightCube.Draw(lightShader);

		renderCaine(myCustomShader);
		updateCameraRotation();
	
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initSkybox();

	glCheckError();

	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(glWindow)) {

		float currentFrame = glfwGetTime();
		delta = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		processMovement();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderScene();

		glfwSwapBuffers(glWindow);
		glfwPollEvents();
	}

	cleanup();

	return 0;
}
