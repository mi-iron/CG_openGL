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

bool	readMesh(const char* filename);
void	deleteMesh();

//camera configuation
vec3	eye(3, 3, 3);
vec3	center(0, 0, 0);
vec3	up(0, 1, 0);

//light configuation
vec4	light(5.0, 5.0, 0.0, 1);

//global coordinate frame
bool	axes = true;

float	AXIS_LENGTH = 3;
float	AXIS_LINE_WIDTH = 2;

//colors
GLfloat	bgColor[4] = { 1,1,1,1 };

//control variable
bool	smooth = true;

//selected example
int		selection = 1;

//sphere, cylinder
GLUquadricObj* sphere = NULL;

// paly configuraion
bool	pause = true;

float	timeStep = 1.0f / 120; //120fps
float	period = 4.0;

//drawing parameters
bool	polygonFill = true;

//current frame
int		frame = 0;

bool isSmoothShading = true; // 부드러운 쉐이딩 기본값
int rotationSegments = 36;   // 기본 회전 샘플 포인트 수
const int maxRotationSegments = 72; // 최대 회전 샘플 포인트 수 (조정 가능)
const int minRotationSegments = 3;  // 최소 회전 샘플 포인트 수 (조정 가능)


int main(int argc, char* argv[]) {
	// vsync should be 0 for precise time stepping
	vsync = 0; 

	//initialize the opengl sys
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	//callbacks
	glfwSetKeyCallback(window, keyboard);

	//depth testing
	glEnable(GL_DEPTH_TEST);

	//normal vectors are normalized after transformation
	glEnable(GL_NORMALIZE);

	//viewport and perspective setting
	reshape(window, windowW, windowH);

	//initialization - main loop - finalization
	//
	init();

	//main loop
	float	previous = (float)glfwGetTime();
	float	elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();		//events

		//time passed during a single loop
		float	now = (float)glfwGetTime();
		float	delta = now - previous;
		previous = now;

		//time passed after the previous frame
		elapsed += delta;

		//deal w/ the current frame
		if (elapsed > timeStep) {
			//rotate the line segment if not paused
			if (!pause) frame += 1;

			elapsed = 0; //reset the elapsed time
		}
		render(window);
		glfwSwapBuffers(window);
	}
	//finalizaion
	quit();

	//terminate the glfw sys
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
void init() {
	//prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GLU_FALSE);

	//prepare mesh
	readMesh("m01_bunny.off");

	//keyboard
	cout << endl;
	cout << "Keyboard Input : space for play/pause" << endl;
	cout << "Keyboard Input : s for turn on/off smooth shading" << endl;
	cout << "Keyboard Input : f for polygon fill on/off" << endl;
	cout << endl;
	cout << "Keyboard Input : 1 for sphere with	16 slices and 16 stacks" << endl;
	cout << "Keyboard Input : 2 for sphere with	32 slices and 32 stacks" << endl;
	cout << "Keyboard Input : 3 for sphere with	64 slices and 64 stacks" << endl;
	cout << "Keyboard Input : 4 for sphere with	128 slices and 128 stacks" << endl;
	cout << "Keyboard Input : 5 for flat approximation of a bunny" << endl;
	cout << "Keyboard Input : 6 for smooth approximation of a bunny" << endl;
}

void quit() {
	//delete quadric shapes
	gluDeleteQuadric(sphere);

	//delete mesh
	deleteMesh();
}

// light
void setupLight() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

//material
void setupMaterial() {
	//material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 128;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void setDiffuseColor(const vec3& color) {
	GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}

void drawSphere(float radius, int slices, int stacks, const vec3& color) {
	//material
	setDiffuseColor(color);

	//sphere using glu quadrics
	gluSphere(sphere, radius, slices, stacks);
}

//read a mesh from a given OFF file
int		nVertices = 0, nFaces = 0, nEdges = 0;
glm::vec3* vertex = NULL;
glm::vec3* vnormal = NULL; //vetex normal
glm::vec3* fnormal = NULL; //face normal
int* face[3] = { NULL, NULL, NULL };

bool readMesh(const char* filename) {
	ifstream	is(filename);
	if (is.fail()) return false;

	char	magicNumber[256];
	is >> magicNumber;

	// # vertices, # faces, # edges
	is >> nVertices >> nFaces >> nEdges;
	cout << "# vertices = " << nVertices << endl;
	cout << "# faces = " << nFaces << endl;

	//vertices
	vertex = new vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		is >> vertex[i].x >> vertex[i].y >> vertex[i].z;

	//vertex normals
	vnormal = new glm::vec3[nVertices];
	for (int i = 0; i < nVertices; i++)
		vnormal[i] = glm::vec3(0, 0, 0);

	//face normals
	fnormal = new glm::vec3[nFaces];

	//faces
	face[0] = new int[nFaces]; //only support triangles
	face[1] = new int[nFaces];
	face[2] = new int[nFaces];

	int n;
	for (int i = 0; i < nFaces; i++) {
		is >> n >> face[0][i] >> face[1][i] >> face[2][i];
		if (n != 3) cout << "# vertices of the " << i << "-th faces = " << n << endl;

		//normal vector of the face
		glm::vec3	v1 = vertex[face[1][i]] - vertex[face[0][i]];
		glm::vec3	v2 = vertex[face[2][i]] - vertex[face[0][i]];
		glm::vec3	v = normalize(cross(v1, v2));

		fnormal[i] = v;

		//add it to the normal vector of each vertex
		vnormal[face[0][i]] += v;
		vnormal[face[1][i]] += v;
		vnormal[face[2][i]] += v;
	}

	//normalization of the normal vectors
	for (int i = 0; i < nVertices; i++)
		vnormal[i] = normalize(vnormal[i]);

	return true;
}

//delete the vertices and faces of the mesh
void deleteMesh() {
	if (vertex) { delete[] vertex; vertex = NULL; }
	if (vnormal) { delete[] vnormal; vnormal = NULL; }
	if (fnormal) { delete[] fnormal; fnormal = NULL; }
	if (face[0]) { delete[] face[0]; face[0] = NULL; }
	if (face[1]) { delete[] face[1]; face[1] = NULL; }
	if (face[2]) { delete[] face[2]; face[2] = NULL; }
}

//draw a flat mesh by specifying its face normal vectors
void drawFlatMesh() {
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++) {
		glNormal3fv(value_ptr(fnormal[i]));
		for (int j = 0; j < 3; j++)
			glVertex3fv(value_ptr(vertex[face[j][i]]));
	}
	glEnd();
}

void drawSmoothMesh() {
	//glShadeModel(GL_SMOOTH);//임시
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
	for (int j = 0; j < 3; j++) {
			glNormal3fv(value_ptr(vnormal[face[j][i]]));
			glVertex3fv(value_ptr(vertex[face[j][i]]));
	}
	glEnd();
}

void render(GLFWwindow* window) {
	//polygon mode
	if (polygonFill)	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//bg color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	//axes
	if (axes) {
		glDisable(GL_LIGHTING);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}
	
	//lighting
	setupLight();

	//smooth shading
	if (smooth) glShadeModel(GL_SMOOTH);
	else glShadeModel(GL_FLAT);

	//material
	setupMaterial();

	//rotation
	float		theta = frame * 4 / period;
	glm::vec3	axis(0, 1, 0);
	glRotatef(theta, axis.x, axis.y, axis.z);

	//draw sphere using gluSphere() after setting up the material
	switch (selection)
	{
	case 1: drawSphere(1.0, 16, 16, vec3(0.95, 0.95, 0.95));			break;
	case 2: drawSphere(1.0, 32, 32, vec3(0.95, 0.95, 0.95));			break;
	case 3: drawSphere(1.0, 64, 64, vec3(0.95, 0.95, 0.95));			break;
	case 4: drawSphere(1.0, 128, 128, vec3(0.95, 0.95, 0.95));		break;
	case 5: drawFlatMesh();																	break;
	case 6: drawSmoothMesh();																break;
	}
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			//quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			//axes
		case GLFW_KEY_X: axes = !axes;	break;
			// play on, off
		case GLFW_KEY_SPACE: pause = !pause; break;
			//control for changing smooth/falt shading
		case GLFW_KEY_S: smooth = !smooth;	break;
			//polygon fill on, off
		case GLFW_KEY_F: polygonFill = !polygonFill; break;

			// example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;
		case GLFW_KEY_5: selection = 5; break;
		case GLFW_KEY_6: selection = 6; break;
		}
	}
}
