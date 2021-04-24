/*
 * MilestoneTwo.cpp
 *
 *  Created on: Aug 16, 2020
 *      Author: cnohi
 */

/* Header Inclusions */
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

/* GLM Math Header Inclusions */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* SOIL Image Loader Inclusion */
#include "SOIL2/SOIL2.h"

using namespace std;	// Standard namespace

#define WINDOW_TITLE "Modern OpenGL"	// Window Title Macro

/* Shader Program Macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Variable declarations for shader, window size initialization, buffer and array objects */
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, texture;

// Subject position and scale
glm::vec3 shapePosition(0.0f, 0.0f, -1.0f);
glm::vec3 shapeScale(2.0f);

//  First light color and position
glm::vec3 lightColor(0.0f, 0.0f, 1.0f);
glm::vec3 lightPosition(0.5f, -0.5f, -3.0f);

// Second light color and position
glm::vec3 secondLightColor(1.0f, 1.0f, 1.0f);
glm::vec3 secondLightPosition(0.0, 3.0f, 3.0f);

bool	perspectiveProj = true;	// Starts true to have the program start with a Perspective projection, 'false' is how we indicate to switch to an Orthographic projection

GLfloat lastMouseX = 400, lastMouseY = 300;	// Locks mouse cursor at the center of the screen
GLfloat mouseXOffset, mouseYOffset, yaw = glm::pi<float>()/2.0f, pitch = 0.0f;	// Mouse offset, yaw, and pitch variable
GLfloat sensitivity = 0.5f;	// Used for mouse / camera rotation sensitivity
bool mouseDetected = true;	// Initially true when mouse movement is detected

GLchar currentKey;		// Will store key pressed

/* Global Vector Declarations */
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);		// Initial camera position
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f);		// Temporary y unit vector
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f);	// Temporary z unit vector
glm::vec3 front;	// Temporary z unit vector for mouse

// Camera rotation
float cameraRotation = glm::radians(-90.0f);

/* Function prototypes */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);
void UMouseClick(int button, int state, int x, int y);
void UMouseMove(int x, int y);
void UMousePressedMove(int x, int y);
void UKeyboard(unsigned char key, int x, int y);
void UKeyReleased(unsigned char key, int x, int y);



/* Vertex Shader Source Code */
const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position;		// Vertex data from Vertex Attrib Pointer 0
		layout (location = 1) in vec3 normal;	// VAP position 1 for normals
		layout (location = 2) in vec2 textureCoordinates;

		out vec3 Normal;	// For outgoing normals to fragment shader
		out vec3 FragmentPos;	// For outgoing color / pixels to fragment shader
		out vec2 mobileTextureCoordinate;

		// Global Variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main(){
			gl_Position = projection * view * model * vec4(position, 1.0);	// Transforms vertices to clip coordinates
			FragmentPos = vec3(model * vec4(position, 1.0f));
			Normal = mat3(transpose(inverse(model))) * normal;
			mobileTextureCoordinate = vec2(textureCoordinates.x, 1.0f - textureCoordinates.y);	// Flips the texture horizontally
		}
);

/* Fragment Shader Source Code */
const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 Normal;		// for incoming normals
		in vec3 FragmentPos; // for incoming fragment positions
		in vec2 mobileTextureCoordinate;

		out vec4 shapeColor;		// For outgoing shape color to the GPU

		// Uniform / Global Variables for object color, light color, light position and camera / view position
		// First Light
		uniform vec3 lightColor;
		uniform vec3 lightPos;
		// Second Light
		uniform vec3 secondLightColor;
		uniform vec3 secondLightPos;

		uniform vec3 viewPosition;
		uniform sampler2D uTexture;	// Useful when working with multiple textures

		vec3 objectColor = texture(uTexture, mobileTextureCoordinate).xyz;

		void main(){

			/* Phong lighting model calculations to generate ambient, diffuse, and secular components */

			// Calculate First Light (Fill Light)
			// Calculate Ambient Lighting
			float ambientStrength = 0.1f;	// Set ambient or global lighting strength
			vec3 ambient = ambientStrength * lightColor;	// Generate ambient light color

			// Calculate diffuse lighting
			vec3 norm = normalize(Normal);	// Normalize vectors to 1 unit
			vec3 lightDirection = normalize(lightPos - FragmentPos);	// Calculate distance (light direction) between light source and fragments / pixels on
			float impact = max(dot(norm, lightDirection), 0.0);			// Calculate diffuse impact by generating dot product of normal and light
			vec3 diffuse = impact * lightColor;							// Generate diffuse light color

			// Calculate Specular Lighting
			float specularIntensity = 0.2f;		// Set specular light strength
			float highlightSize = 16.0f;		// Set specular highlight size
			vec3 viewDir = normalize(viewPosition - FragmentPos);	// Calculate view direction
			vec3 reflectDir = reflect(-lightDirection, norm);		// Calculate reflection vector
			// Calculate specular component
			float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
			vec3 specular = specularIntensity * specularComponent * lightColor;

			// Calculate Phong Result with first light
			vec3 phong = (ambient + diffuse + specular) * objectColor;


			// Calculate Second Light (Key Light)
			// Calculate Ambient Lighting
			ambientStrength = 0.1f;
			ambient = ambientStrength * secondLightColor;

			// Calculate Diffuse Lighting
			lightDirection = normalize(secondLightPos - FragmentPos);	// Calculate distance (light direction) between light source and fragments / pixels on
			impact = max(dot(norm, lightDirection), 0.0);				// Calculate diffuse impact by generating dot product of normal and light
			diffuse = impact * secondLightColor;						// Generate diffuse light color

			// Calculate Specular Lighting
			specularIntensity = 0.8f;		// Set specular light strength
			highlightSize = 8.0f;			// Set specular highlight size
			reflectDir = reflect(-lightDirection,norm);
			// Calculate Specular Component
			specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
			specular = specularIntensity * specularComponent * secondLightColor;

			// Calculate Phong result with first and second light
			phong += (ambient + diffuse + specular) * objectColor;

			shapeColor = vec4(phong, 1.0f);
		}
);

/* Main Program */
int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	cout << "Use the 'V' key to switch between Perspective and Orthographic views." << endl;

	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK){
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	UCreateShader();
	UCreateBuffers();
	UGenerateTexture();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// Sets the background color to black
	glutDisplayFunc(URenderGraphics);

	glutMouseFunc(UMouseClick);			// Detects Mouse Click
	glutPassiveMotionFunc(UMouseMove);	// Detects Mouse Movement
	glutMotionFunc(UMousePressedMove);	// Detects Mouse Press and Movement

	glutKeyboardFunc(UKeyboard);				// Detects Key Press
	glutKeyboardUpFunc(UKeyReleased);			// Detects Key Release

	glutMainLoop();

	// Destroys Buffer objects once used
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

/* Resizes the Window */
void UResizeWindow(int w, int h){
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

/* Renders Graphics */
void URenderGraphics(void){
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clears the screen

	GLint modelLoc, viewLoc, projLoc, lightColorLoc, lightPositionLoc, secondLightColorLoc, secondLightPositionLoc, viewPositionLoc;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	/* use the Shader and activate the Vertex Array Object for rendering and transforming */
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);	// Activate the Vertex Array Object before drawing

	CameraForwardZ = front;	// Replaces camera forward vector with Radians normalized as a unit vector

	// Transforms the object
	model = glm::translate(model, shapePosition);	// Place the object at the center of the viewport
	model = glm::scale(model, shapeScale);	// Increase the object size by a scale of 2

	// Transforms the camera
	//view = glm::translate(view, cameraPosition);	// moves the world -5 units in the z
	view = glm::lookAt(cameraPosition - CameraForwardZ, cameraPosition, CameraUpY);

	// Defaults to the Perspective projection but we can switch between the two projections
	if(perspectiveProj){
		// Creates a perspective projection
		projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);
	} else {
		// Creates a orthographic projection
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}

	// Retrieves and passes transform matrices to the Shader program
		modelLoc = glGetUniformLocation(shaderProgram, "model");
		viewLoc = glGetUniformLocation(shaderProgram, "view");
		projLoc = glGetUniformLocation(shaderProgram, "projection");

		// Pass matrix data to the shader program's matrix uniforms
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Reference matrix uniforms from the shader program for the shape color, light color, light position and view position
		lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");				// First Light Color
		lightPositionLoc = glGetUniformLocation(shaderProgram, "lightPos");				// First Light Position
		secondLightColorLoc = glGetUniformLocation(shaderProgram, "secondLightColor");	// Second Light Color
		secondLightPositionLoc = glGetUniformLocation(shaderProgram, "secondLightPos");	// Second Light Position
		viewPositionLoc = glGetUniformLocation(shaderProgram, "viewPosition");

		// Pass color, light and camera data to the shader program's corresponding uniforms
		glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
		glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(secondLightColorLoc, secondLightColor.r, secondLightColor.g, secondLightColor.b);
		glUniform3f(secondLightPositionLoc, secondLightPosition.x, secondLightPosition.y, secondLightPosition.z);
		glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


		glBindTexture(GL_TEXTURE_2D, texture);	// Activate the texture

		// Draw triangles
		glDrawArrays(GL_TRIANGLES, 0, 300);

		glBindVertexArray(0);	// Deactivate the Vertex Array Object

		glutPostRedisplay();
		glutSwapBuffers();	// Flips the back buffer with the front buffer every frame. Similar to GL Flush
}

/* Creates the Shader Program */
void UCreateShader(){
	// Vertex Shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);	// Creates the Vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);	// Attaches the Vertex shader to the source code
	glCompileShader(vertexShader);	// Compiles the Vertex Shader

	// Fragment Shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	// Creates the fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);	// Attaches the Fragment shader to the source code
	glCompileShader(fragmentShader);	// Compiles the Fragment shader

	shaderProgram = glCreateProgram();	// Creates the shader program and returns an id
	glAttachShader(shaderProgram, vertexShader);	//Attach Vertex shader to the Shader Program
	glAttachShader(shaderProgram, fragmentShader);	// Attach Fragment shader to the Shader Program
	glLinkProgram(shaderProgram);	// Links Vertex and Fragment shader to Shader Program

	// Delete the Vertex and Fragment shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UCreateBuffers(){
	GLfloat vertices[] = {
				// Positions			// Normals				// Texture Coords
				// Left Sides
				-0.55f, -1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.55f, -0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.55f, -0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, back left leg
				-0.55f, -0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.55f, -1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				-0.55f, -0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.55f,  0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.55f,  0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, chair seat
				-0.55f,  0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.55f, -0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				-0.55f, -1.0f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.55f, -0.1f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.55f, -0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, front left leg
				-0.55f, -0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.55f, -1.0f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -1.0f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				-0.55f,  0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.55f,  1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.55f,  1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, chair back on the left
				-0.55f,  1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.55f,  0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.55f,  0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.45f,  0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.45f,  1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, chair back on the right
				 0.45f,  1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.45f,  0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.45f, -1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.45f, -0.1f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.45f, -0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, Back left leg
				 0.45f, -0.1f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.45f, -1.0f, -0.45f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.45f, -1.0f, -0.55f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.45f, -1.0f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.45f, -0.1f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.45f, -0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Left side, front left leg
				 0.45f, -0.1f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.45f, -1.0f,  0.55f,	-1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.45f, -1.0f,  0.45f,	-1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 // Right Sides
				-0.45f, -1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.45f, -0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.45f, -0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, Back left leg
				-0.45f, -0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.45f, -1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.45f, -1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.55f, -0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.55f,  0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.55f,  0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, chair seat
				 0.55f,  0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.55f, -0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.55f, -0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				-0.45f, -1.0f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.45f, -0.1f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.45f, -0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, front left leg
				-0.45f, -0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.45f, -1.0f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.45f, -1.0f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,


				-0.45f,  0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				-0.45f,  1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, chair back on the left
				-0.45f,  1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				-0.45f,  0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.55f,  0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.55f,  1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.55f,  1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, chair back on the right
				 0.55f,  1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.55f,  0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.55f,  0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,


				 0.55f, -1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.55f, -0.1f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.55f, -0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, Back left leg
				 0.55f, -0.1f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.55f, -1.0f, -0.45f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.55f, -1.0f, -0.55f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 0.55f, -1.0f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,
				 0.55f, -0.1f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 1.0f,
				 0.55f, -0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,	// Right side, front left leg
				 0.55f, -0.1f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 1.0f,
				 0.55f, -1.0f,  0.55f,	 1.0f,  0.0f,  0.0f,	1.0f, 0.0f,
				 0.55f, -1.0f,  0.45f,	 1.0f,  0.0f,  0.0f,	0.0f, 0.0f,

				 // Front Sides
				-0.55f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.55f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				-0.45f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, back left leg
				-0.45f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				-0.45f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.55f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.55f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.55f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				-0.45f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, front left leg
				-0.45f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				-0.45f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.55f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				 0.55f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				 0.55f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, back right leg
				 0.45f, -0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				 0.55f, -1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				 0.55f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				 0.55f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, front right leg
				 0.45f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				 0.55f, -1.0f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.55f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.55f,  0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.55f,  0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair seat
				 0.55f,  0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.55f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.55f, -0.1f,  0.55f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.55f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.55f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				-0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back on left
				-0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				-0.45f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.55f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				 0.55f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				 0.55f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back on right
				 0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				 0.55f,  0.1f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.45f,  0.9f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back top beam
				 0.45f,  1.0f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f,  0.9f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.45f,  0.9f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.45f,  0.7f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.45f,  0.8f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f,  0.8f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back first middle beam
				 0.45f,  0.8f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f,  0.7f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.45f,  0.7f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.45f,  0.5f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.45f,  0.6f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f,  0.6f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back second middle beam
				 0.45f,  0.6f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f,  0.5f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.45f,  0.5f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				-0.45f,  0.3f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,
				-0.45f,  0.4f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 1.0f,
				 0.45f,  0.4f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,	// Front side, chair back third middle beam
				 0.45f,  0.4f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 1.0f,
				 0.45f,  0.3f, -0.45f,	 0.0f,  0.0f,  1.0f,	1.0f, 0.0f,
				-0.45f,  0.3f, -0.45f,	 0.0f,  0.0f,  1.0f,	0.0f, 0.0f,

				 // Back Sides
				-0.55f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.55f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				-0.45f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, back left leg
				-0.45f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				-0.45f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.55f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.55f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.55f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				-0.45f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, front left leg
				-0.45f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				-0.45f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.55f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				 0.55f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				 0.55f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, back right leg
				 0.45f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				 0.55f, -1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				 0.55f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				 0.55f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, front right leg
				 0.45f, -0.1f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				 0.55f, -1.0f,  0.45f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.55f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair seat
				 0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.55f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.55f, -0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.55f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				-0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back on left
				-0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				-0.45f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				 0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				 0.55f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back on right
				 0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				 0.55f,  0.1f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.45f,  0.9f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back top beam
				 0.45f,  1.0f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f,  0.9f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.45f,  0.9f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.45f,  0.7f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.45f,  0.8f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f,  0.8f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back first middle beam
				 0.45f,  0.8f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f,  0.7f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.45f,  0.7f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.45f,  0.5f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.45f,  0.6f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f,  0.6f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back second middle beam
				 0.45f,  0.6f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f,  0.5f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.45f,  0.5f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				-0.45f,  0.3f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
				-0.45f,  0.4f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
				 0.45f,  0.4f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,	// Back side, chair back third middle beam
				 0.45f,  0.4f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
				 0.45f,  0.3f, -0.55f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
				-0.45f,  0.3f, -0.55f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

				 // Top sides
				-0.55f,  0.1f,  0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
				-0.55f,  0.1f, -0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
				 0.55f,  0.1f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,	// Top side, chair seat
				 0.55f,  0.1f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
				 0.55f,  0.1f,  0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
				-0.55f,  0.1f,  0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,

				-0.55f,  1.0f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
				-0.55f,  1.0f, -0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
				 0.55f,  1.0f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,	// Top side, chair back
				 0.55f,  1.0f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
				 0.55f,  1.0f, -0.45f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
				-0.55f,  1.0f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.8f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.8f, -0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.8f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,	// Top side, chair back first middle beam
				 0.45f,  0.8f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.8f, -0.45f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.8f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.6f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.6f, -0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.6f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,	// Top side, chair back second middle beam
				 0.45f,  0.6f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.6f, -0.45f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.6f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.4f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.4f, -0.55f,	 0.0f,  1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.4f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,	// Top side, chair back third middle beam
				 0.45f,  0.4f, -0.55f,	 0.0f,  1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.4f, -0.45f,	 0.0f,  1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.4f, -0.45f,	 0.0f,  1.0f,  0.0f,	0.0f, 0.0f,

				// Bottom sides
				-0.55f, -0.1f,  0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.55f, -0.1f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.55f, -0.1f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, chair seat
				 0.55f, -0.1f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.55f, -0.1f,  0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -0.1f,  0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.9f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.9f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.9f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, chair back top beam
				 0.45f,  0.9f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.9f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.9f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.7f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.7f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.7f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, chair back first middle beam
				 0.45f,  0.7f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.7f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.7f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.5f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.5f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.5f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, chair back second middle beam
				 0.45f,  0.5f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.5f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.5f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.45f,  0.3f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.45f,  0.3f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f,  0.3f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, chair back third middle beam
				 0.45f,  0.3f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f,  0.3f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.45f,  0.3f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.55f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.55f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				-0.45f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, back left leg
				-0.45f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				-0.45f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				-0.55f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				-0.55f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				-0.45f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, front left leg
				-0.45f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				-0.45f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				-0.55f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				 0.55f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				 0.55f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, back right leg
				 0.45f, -1.0f, -0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				 0.55f, -1.0f, -0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

				 0.55f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,
				 0.55f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	0.0f, 1.0f,
				 0.45f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,	// Bottom side, front right leg
				 0.45f, -1.0f,  0.55f,	 0.0f, -1.0f,  0.0f,	1.0f, 1.0f,
				 0.45f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	1.0f, 0.0f,
				 0.55f, -1.0f,  0.45f,	 0.0f, -1.0f,  0.0f,	0.0f, 0.0f,

		};

	// Generate Buffer IDs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Activate the VAO before binding and setting VBOs and VAPs
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// Copy vertices to VBO

	// Set attribute pointer 0 to hold Position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set attribute pointer 1 to hold Normal data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Set attribute pointer 2 to hold Color data. NOte that color is not used
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);	// Unbind the VAO
}

/* Generate and load the texture */
void UGenerateTexture(){
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;

	unsigned char* image = SOIL_load_image("WoodTexture.jpg", &width, &height, 0, SOIL_LOAD_RGB);	// Loads texture file

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);	// Unbind the texture
}

/* Implements the UMouseClick Function */
void UMouseClick(int button, int state, int x, int y){
}

/* Implements the UMouseMove Function */
void UMouseMove(int x, int y){
	// Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected){
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}
	// Gets the direction the mouse was move in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y; // Inverted y

	// Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	// Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	// Accumulates the yaw and pitch variables
	yaw += mouseXOffset;
	if (glm::radians(pitch + mouseYOffset) >= glm::radians(-89.0f) && glm::radians(pitch + mouseYOffset) <= glm::radians(89.0f)){
		pitch += mouseYOffset;
	}

	// Orbits around the center
	front.x = 10.0f * cos(glm::radians(yaw));
	front.y = 10.0f * sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * 10.0f;
}

/* Implements the UMousePressedMove function */
void UMousePressedMove(int x, int y){
	UMouseMove(x,y);
}

/* Implements the UKeyboard Function */
void UKeyboard(unsigned char key, GLint x, GLint y){
	switch(key){
	case 'v':
		currentKey = key;
		perspectiveProj = !perspectiveProj;
		break;
	}
}

/* Implements the UKeyReleased Function */
void UKeyReleased(unsigned char key, GLint x, GLint y){
	currentKey = '0';
}





