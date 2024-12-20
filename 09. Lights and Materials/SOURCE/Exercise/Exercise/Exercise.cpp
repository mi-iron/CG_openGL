#include "glSetup.h"

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void	init();
void	quit();
void	render(GLFWwindow* window);
void	keyboard(GLFWwindow* window, int key, int code, int action, int mods);

//camera configuation
vec3	eyeTopView(0, 10, 0); //top view
vec3	upTopView(0, 0, -1);

vec3	eyeFrontView(0, 0, 10); //front view
vec3	upFrontView(0, 1, 0);

vec3	eyeBirdView(0, -10, 4); //bird-eye view
vec3	upBirdView(0, 1, 0);
vec3	center(0, 0, 0);

//light configuraion
vec4	ligntInitialP(0.0, 0.0, 0.8, 1);

//global coordinate frame
bool	axes = true;

float	AXIS_LENGTH = 1.25;
float	AXIS_LINE_WIDTH = 2;

//colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

//control variable
int		view = 1;
int		selection = 1;

bool	rotationObject = false; //rotate the models
bool	rotationLight = false; //rotate the lights

float	thetaModel = 0; //rotationa angle around the y-axis
float	thetaLight[3];

bool	lightOn[3]; //point, distant, spot lights
bool	attenuation = false; //attenuation for point light

int		material = 0; //predefined material parameters

bool	pause = true;

float	timeStep = 1.0f / 120;
float	period = 4.0f;

int		frame = 0;

void reinitialize() {
	frame = 0;

	lightOn[0] = true; //turn on only the point light
	lightOn[1] = false;
	lightOn[2] = false;

	thetaModel = 0;
	for (int i = 0; i < 3; i++)
		thetaLight[i] = 0;
}

void animate()
{
	frame += 1;

	//rotation angle of the light
	if (rotationLight) {
		for (int i = 0; i < 3; i++)
			if (lightOn[i]) thetaLight[i] += 4 / period; //degree
	}

	//rotate angle the models
	if (rotationObject) thetaModel += 4 / period; //degree

}

int main(int argc, char* argv[]) {
	//vsync should be 0 for precise time stepping
	vsync = 0;

	//field of view of 85mm lens in degree
	fovy = 16.1f;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	glfwSetKeyCallback(window, keyboard);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	reshape(window, windowW, windowH);

	init();

	float	previous = (float)glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float	now = (float)glfwGetTime();
		float	delta = now - previous;
		previous = now;

		elapsed += delta;

		if (elapsed > timeStep) {
			if (!pause) animate();

			elapsed = 0; //reset the elapsed time
		}

		render(window);				//draw one frame
		glfwSwapBuffers(window);	// swap buffers
	}

	quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

GLUquadricObj* sphere = NULL;
void init() {
	//animation system
	reinitialize();

	//prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GLU_FALSE);

	//keyboard
	cout << endl;
	cout << "Keyboard Input : space for play/pause" << endl;
	cout << "Keyboard Input : i for reinitialization" << endl;
	cout << "Keyboard Input : v for top, front, bird-eye view" << endl;
	cout << endl;
	cout << "Keyboard Input : p for a point light" << endl;
	cout << "Keyboard Input : a for light attenuation" << endl;
	cout << "Keyboard Input : e for time dependent exponent of a spot light" << endl;
	cout << "Keyboard Input : l for rotation of lights" << endl;
	cout << "Keyboard Input : o for rotation of objects" << endl;

	cout << "Keyboard Input : up for increasing period" << endl;
	cout << "Keyboard Input : down for decreasing period" << endl;
	cout << "Keyboard Input : left for decreasing shininess coefficient" << endl;
	cout << "Keyboard Input : right for increasing shininess coefficient" << endl;

	
}

void quit() {
	//delete quadric shapes
	gluDeleteQuadric(sphere);

}

//material shininess coefficient
GLfloat mat_shininess = 0;

void setupWhiteShinyMaterial() {
	//material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}


//draw objects using a GLU quadric
void drawSphere(float radius, int slices, int stacks) {

	gluSphere(sphere, radius, slices, stacks);
}


//compute the rotation axis and angle from a to b
//axis is not normalized
//theta is represented in degrees

void computeRotation(const vec3& a, const vec3& b, float& theta, vec3& axis) {
	axis = cross(a, b);
	float	sinTheta = length(axis);
	float	cosTheta = dot(a, b);
	theta = float(atan2(sinTheta, cosTheta) * 180.0 / M_PI);
}

//light
void setupLight(const vec4& p, int i) {
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	//inte
	glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + i, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + i, GL_POSITION, value_ptr(p));

	//attenuation for the point and spot light
	if ((i == 0 || i == 2) && attenuation)
	{
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.1f);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.05f);
	}
	else
	{
		glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 0.0f);
	}

	//Point and distant light
		// 180 to turn off cutoff when it has been used as a spot light.
	glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180);
	
}


void drawArrow(const vec3& p, bool tailOnly) {
	//make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//common material
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess_ = 25;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess_);

	//transformation
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	if (!tailOnly) {
		float	theta;
		vec3	axis;
		computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
		glRotatef(theta, axis.x, axis.y, axis.z);
	}

	//tail sphere
	float	arrowTailRadius = 0.05f;
	glColor3f(1, 0, 0);
	drawSphere(arrowTailRadius, 16, 16);


	glPopMatrix();

	//for convential material setting
	glDisable(GL_COLOR_MATERIAL);
}


void render(GLFWwindow* window) {

	//bg color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec3	eye(0), up(0);
	switch (view)
	{
	case	0: eye = eyeTopView; up = upTopView; break;
	case	1: eye = eyeFrontView; up = upFrontView; break;
	case	2: eye = eyeBirdView; up = upBirdView; break;
	}

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	//axes
	if (axes) {
		glDisable(GL_LIGHTING);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	glShadeModel(GL_SMOOTH);

	//rotation of the light or 3x3 models
	vec3	axis(0, 1, 0);

	//lighting
	glEnable(GL_LIGHTING);

	//set up the lights
	vec4	lightP[3];
	for (int i = 0; i < 3; i++) {
		//just turn off the i-th light, if not lit
		if (!lightOn[i]) { glDisable(GL_LIGHT0 + i); continue; }

		//turn on the i-th light
		glEnable(GL_LIGHT0 + i);

		//dealing with the distant light
		lightP[i] = ligntInitialP;
		if (i == 1) lightP[i].w = 0;

		//lights rotate around the center of the world coordinate system
		mat4	R = rotate(mat4(1.0), radians(thetaLight[i]), axis);
		lightP[i] = R * lightP[i];

		//set up the i-th light
		setupLight(lightP[i], i);
	}

	//draw the geometries of the lights
	for (int i = 0; i < 3; i++) {
		if (!lightOn[i])		continue;
		drawArrow(lightP[i], i == 0);
	}

	//draw objects
	setupWhiteShinyMaterial();

	glScalef(0.4f, 0.4f, 0.4f);

	vec3 u[3]; // y-axis
	vec3 v[3]; // z-axis
	u[0] = vec3(0, 1, 0) * 2.0f;	u[1] = vec3(0, 0, 0); u[2] = -u[0];
	v[0] = -vec3(1, 0, 0) * 4.0f;	v[1] = vec3(0, 0, 0); v[2] = -v[0];

	for (int i = 0; i < 3; i++)
	{
		glPushMatrix();
		glTranslatef(u[i].x, u[i].y, u[i].z);

		for (int j = 0; j < 3; j++)
		{
			glPushMatrix();

			glTranslatef(v[j].x, v[j].y, v[j].z);
			glRotatef(thetaModel, axis.x, axis.y, axis.z);

			switch (selection)
			{
			case 1: drawSphere(0.7f, 64, 64); break;
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			//quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

		case GLFW_KEY_X: axes = !axes;		break;

		case GLFW_KEY_I: reinitialize();	break;

		case GLFW_KEY_SPACE: pause = !pause; break;

		case GLFW_KEY_UP: period += 0.1f;	break;
		case GLFW_KEY_DOWN: period = (float)std::max(period - 0.1, 0.1);	break;

		case GLFW_KEY_V: view = (view + 1) % 3;	break;
		case GLFW_KEY_P: lightOn[0] = !lightOn[0];	break;
		case GLFW_KEY_A: attenuation = !attenuation;		break;
		case GLFW_KEY_L: rotationLight = !rotationLight;	break;
		case GLFW_KEY_O: rotationObject = !rotationObject;	break;

		case GLFW_KEY_LEFT: 
			if (mat_shininess > 0) mat_shininess /= 2;
			else if (mat_shininess == 1) mat_shininess -= 1;
			break;
		case GLFW_KEY_RIGHT: 
			if (mat_shininess == 0) mat_shininess += 1;
			else if (mat_shininess > 128) mat_shininess = 128;
			else mat_shininess *= 2;
			break;

		}
	}
}