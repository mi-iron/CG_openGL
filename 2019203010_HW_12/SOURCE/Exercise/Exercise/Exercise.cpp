#include "glSetup.h"

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
void	mouseButton(GLFWwindow* window, int button, int action, int mods);
void	mouseMove(GLFWwindow* window, double x, double y);

//camera configuation
vec3	eyeTopView(0, 10, 0); //top view
vec3	upTopView(0, 0, -1);

vec3	eyeFrontView(0, 0, 10); //front view
vec3	upFrontView(0, 1, 0);

vec3	eyeBirdView(0, -10, 4); //bird-eye view
vec3	upBirdView(0, 1, 0);
vec3	center(0, 0, 0);

//light configuraion
vec4	light(0.0, 0.0, 0.8, 1);

//global coordinate frame
bool	axes = true;
float	AXIS_LENGTH = 1.25;
float	AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1,1,1,1 };

// Control variable
int view = 1; // Top, front, bird-eye view

// Picking
bool picked[9] = { 0,0,0,0,0,0,0,0,0 };

//points for the line segments
double point[2][2] = { {0, 0}, {0, 0} };
float square_p[4][2] = { {0, 0}, {0, 0}, {0,0}, {0,0} };

//Mouse input mode
enum class InputMode
{
	NONE = 0,
	DRAGGING = 1,
	COMPLETE = 2,
};

InputMode inputmode = InputMode::NONE;


int main(int argc, char* argv[]) {
	//field of view of 85mm lens in degree
	fovy = 16.1f;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	reshape(window, windowW, windowH);

	init();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		render(window);				//draw one frame
		glfwSwapBuffers(window);

	}
	quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

//sphere
GLUquadric* sphere = NULL;

void init()
{
	//prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GLU_FALSE);


	//keyboard
	cout << endl;
	cout << "Keyboard Input : x for axes on/off" << endl;
	cout << "Mouse dragging : Select objects" << endl;
	cout << endl;
}

void quit()
{
	//delete quadric shapes
	gluDeleteQuadric(sphere);
}

//material
void setupColoredMaterial(const vec3& color) {
	//material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { color[0], color[1],color[2], 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// draw objects using a GLU quadric
void drawSphere(float radius, int slices, int stacks)
{
	gluSphere(sphere, radius, slices, stacks);
}

//light
void setupLight(const vec4& p) {
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));
}

void render(GLFWwindow* wubdiw)
{
	//picking
	glInitNames();	//initialize the name stack
	glPushName(-1);	//push at least one name in the stack

	//bg color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vec3	eye(0), up(0);
	//eye = eyeFrontView; up = upFrontView; 
	switch (view)
	{
	case 0: eye = eyeTopView; up = upTopView; break;
	case 1: eye = eyeFrontView; up = upFrontView; break;
	case 2: eye = eyeBirdView; up = upBirdView; break;
	}


	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	//axes
	if (axes) {
		glDisable(GL_LIGHTING);
		glDisable(GL_LINE_STIPPLE);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	//mouse dragging -> stippled square
	if (inputmode == InputMode::DRAGGING)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);
	}
	else glDisable(GL_LINE_STIPPLE);

	if (inputmode == InputMode::DRAGGING)
	{

		glBegin(GL_LINE_LOOP);
		glVertex2f(square_p[0][0], square_p[0][1]);
		glVertex2f(square_p[1][0], square_p[1][1]);
		glVertex2f(square_p[2][0], square_p[2][1]);
		glVertex2f(square_p[3][0], square_p[3][1]);
		glEnd();
	}


	//smooth shading
	glShadeModel(GL_SMOOTH);

	//lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	setupLight(light);

	//draw objects
	//3x3 spheres or bunnies
	glScalef(0.4f, 0.4f, 0.4f);

	vec3	u[3];	//y-axis
	vec3	v[3];	//x-axis
	u[0] = vec3(0, 1, 0) * 2.0f;	u[1] = vec3(0, 0, 0);	u[2] = -u[0];
	v[0] = -vec3(1, 0, 0) * 4.0f;	v[1] = vec3(0, 0, 0);	v[2] = -v[0];

	for (int i = 0; i < 3; i++)
	{
		glPushMatrix();
		glTranslatef(u[i].x, u[i].y, u[i].z);

		for (int j = 0; j < 3; j++)
		{
			glPushMatrix();
			glTranslatef(v[j].x, v[j].y, v[j].z);

			//material
			if (picked[3 * i + j])	setupColoredMaterial(vec3(0, 0, 1));	//selected
			else setupColoredMaterial(vec3(1, 1, 1));	//non-selected

			//picking
			glLoadName(3 * i + j);	//replace the name fo the i-th row, j-th column

			//draw a sphere
			drawSphere(0.7f, 64, 64);

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

		case GLFW_KEY_V: view = (view + 1) % 3;	break;
		}
	}
}

int findSelected(int select, GLuint selectBuffer[64])
{
	bool	diagnosis = true;

	for (int i = 0; i < 9; i++)
		picked[i] = 0;

	if (diagnosis)	cout << "selections = " << select << endl;

	int		index = 0;
	for (int i = 0; i < select; i++)
	{
		int		n = selectBuffer[index + 0];						//# of names

		if (selectBuffer[index + 3] >= 0 && selectBuffer[index + 3] <= 8) // if selected
		{
			cout << "\t# of names = " << n << endl;
			cout << "\tnames: ";
			for (int j = 0; j < n; j++)
				cout << selectBuffer[index + 3 + j] << " "; //name print out
			cout << endl;
			picked[selectBuffer[index + 3]] = 1; // picked!!(=selected)
		}

		//to the next available one
		index += (3 + n);
	}
	for (int i = 0; i < 9; i++)
	{
		if (picked[i]) cout << i << " ";
	}
	cout << endl;
}

int selectObjects(GLFWwindow* window) //CHANGING PART
{
	for (int i = 0; i < 9; i++)
		picked[i] = 0;
	//width and height of picking region in window coordinates
	double	delX = abs(point[0][0] - point[1][0]);
	double	delY = abs(point[0][1] - point[1][1]);

	double x = (point[0][0] + point[1][0]) / 2.0; //ÁÖÀÇ!
	double y = (point[0][1] + point[1][1]) / 2.0;

	//maximum 64 selections
	GLuint	selectBuffer[64];
	glSelectBuffer(64, selectBuffer);

	//current viewport
	GLint	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//backup the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity();

	//creates a projection matrix that can be used to restrict drawing to
	//a small region of the viewport
	gluPickMatrix(x, viewport[3] - y, delX, delY, viewport);	//y screen to viewport

	//exploit the projection matrix for normal rendering
	setupProjectionMatrix();

	//enter selection mode
	glRenderMode(GL_SELECT);

	//render the objects for selection
	render(window);

	//return to normal rendering mode and getting the picked object
	GLint	selected = glRenderMode(GL_RENDER); 
	int name = findSelected(selected, selectBuffer);

	//restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void worldCoordinate(float xs, float ys, float& xw, float& yw)
{
	// In the world space. See reshape() in glSetup.cpp
	float aspect = (float)screenW / screenH;
	xw = 3.0f * (xs / (screenW - 1) - 0.5f) * aspect;
	yw = -3.0f * (ys / (screenH - 1) - 0.5f);
}

void Setsquare_p(float p1[2], float p2[2]) //stippled square points
{
	if ((p1[0] <= p2[0] && p1[1] <= p2[1]) || (p1[0] >= p2[0] && p1[1] >= p2[1])) {
		square_p[1][0] = p1[0];	square_p[1][1] = p2[1];
		square_p[3][0] = p2[0];	square_p[3][1] = p1[1];
	}
	else if ((p1[0] <= p2[0] && p1[1] >= p2[1]) || (p1[0] >= p2[0] && p1[1] <= p2[1])) {
		square_p[1][0] = p2[0];	square_p[1][1] = p1[1];
		square_p[3][0] = p1[0];	square_p[3][1] = p2[1];
	}
}

void mouseButton(GLFWwindow* window, int button, int action, int mods) //CHANGING PART
{
	//mouse cursor position in the screen coordinate
	double	xs, ys;
	glfwGetCursorPos(window, &xs, &ys);

	float xw, yw;
	worldCoordinate((float)xs, (float)ys, xw, yw);

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputmode = InputMode::DRAGGING;

		//
		square_p[0][0] = xw; square_p[0][1] = yw;
		square_p[2][0] = xw; square_p[2][1] = yw;

		Setsquare_p(square_p[0], square_p[2]);
		point[0][0] = xs; point[0][1] = ys;
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputmode = InputMode::COMPLETE;

		//end point
		square_p[2][0] = xw; square_p[2][1] = yw;

		Setsquare_p(square_p[0], square_p[2]);
		point[1][0] = xs; point[1][1] = ys;
		selectObjects(window);
	}
}

void mouseMove(GLFWwindow* window, double x, double y)
{
	if (inputmode == InputMode::DRAGGING)
	{
		worldCoordinate((float)x, (float)y, square_p[2][0], square_p[2][1]);

		Setsquare_p(square_p[0], square_p[2]);
	}
}
