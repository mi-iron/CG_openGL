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

// camera configuation
struct Camera
{
	vec3	e; //eye position
	vec3	a; //at position
	vec3	u; //up position

	vec3	f; //forward position
	vec3	r; //right position
}	C;

//light configuraion
vec4	light(0.0, 0.0, 0.8, 0);	//light position

//global coordinate frame
float	AXIS_LENGTH = 1.25;
float	AXIS_LINE_WIDTH = 2;

//colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

// control variable
int		selection = 2;

//predefined filed of view angles in the y direction
float	fovys[6] = { 46.4f, 37.8f, 27.0f, 16.1f, 11.4f, 6.9f };
int		m_fovys = 6;
int		i_fovys = 1;

int main(int argc, char* argv[]) {
	//orthographic projection
	perspectiveView = true;

	//initial camera setting
	C.e = vec3(0, 0, 3);	//eye position
	C.a = vec3(0, 0, 0);	//at position
	C.u = vec3(0, 1, 0);	//up vector (given)

	//field of view of 35mm lens in degree
	fovy = fovys[i_fovys];

	//initialize the opengl sys.
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	glfwSetKeyCallback(window, keyboard);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);

	reshape(window, windowW, windowH);

	init();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		render(window);				//draw one frame
		glfwSwapBuffers(window);	// swap buffers
	}

	quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


// sphere, cylinder
GLUquadricObj* sphere = NULL;

void init() {
	//forward and backwoard direction for navigation
	C.f = normalize(C.a - C.e);				// forward direction
	C.r = normalize(cross(C.f, C.u));		// right direction
	C.u = normalize(cross(C.r, C.f));		// up vector (orthogonal to f and r)

	//prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GLU_FALSE);

	readMesh("m01_bunny.off");

	//keyboard
	cout << endl;
	cout << "Keyboard Input : up, down, left, right for viewing" << endl;
	cout << "Keyboard Input : p for perspective/orthographic viewing" << endl;
	cout << "Keyboard Input : f to change field of view angle" << endl;
	cout << endl;
	
	cout << "Keyboard Input : 1 for a sphere" << endl;
	cout << "Keyboard Input : 2 for a flat bunny" << endl;
	cout << "Keyboard Input : 3 for a smooth bunny" << endl;
}

void quit() {
	//delete quadric shapes
	gluDeleteQuadric(sphere);

	//delete mesh
	deleteMesh();
}

//material
void setupColoredMaterial(const vec3& color) {
	//material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { color[0], color[1],color[2], 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// draw objects using a GLU quadric
void drawSphere(float radius, int slices, int stacks) {

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
	//glShadeModel(GL_SMOOTH);//юс╫ц
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
		for (int j = 0; j < 3; j++) {
			glNormal3fv(value_ptr(vnormal[face[j][i]]));
			glVertex3fv(value_ptr(vertex[face[j][i]]));
		}
	glEnd();
}

// light
void setupLight(const vec4& p) {
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));
}

void render(GLFWwindow* window) {
	//bg color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(C.e[0], C.e[1], C.e[2], C.a[0], C.a[1], C.a[2], C.u[0], C.u[1], C.u[2]);

	//axes
	glDisable(GL_LIGHTING);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	//smooth shading
	glShadeModel(GL_SMOOTH);
	
	//lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	setupLight(light);

	//draw objects
	setupColoredMaterial(vec3(0.95, 0.95, 0.95));

	glPushMatrix();

	glScalef(0.7f, 0.7f, 0.7f);

	switch (selection) {
	case 1: drawSphere(0.7f, 64, 64);	break;
	case 2: drawFlatMesh();				break;
	case 3: drawSmoothMesh();			break;
	}

	glPopMatrix();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			//quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			//perspective/orthographic view
		case GLFW_KEY_P:
			perspectiveView = !perspectiveView;
			reshape(window, windowW, windowH);
			break;
			//field of view angle in the y direction
		case GLFW_KEY_F:
			i_fovys = (i_fovys + 1) % m_fovys;
			fovy = fovys[i_fovys];
			reshape(window, windowW, windowH);
			break;

			// example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		}
	}
	
	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && !(mods & GLFW_MOD_SHIFT))
	{
		switch (key)
		{
			//navigation
		case GLFW_KEY_UP:		C.e += 0.1f * C.f;	C.a += 0.1f * C.f;	break;
		case GLFW_KEY_DOWN:		C.e -= 0.1f * C.f;	C.a -= 0.1f * C.f;	break;
		case GLFW_KEY_LEFT:		C.e -= 0.1f * C.r;	C.a -= 0.1f * C.r;	break;
		case GLFW_KEY_RIGHT:	C.e += 0.1f * C.r;	C.a += 0.1f * C.r;	break;
		}
	}

	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && !(mods & GLFW_MOD_SHIFT))
	{
		switch (key)
		{
			//navigation
		case GLFW_KEY_LEFT:		cout << "left turn" << endl;	break;
		case GLFW_KEY_RIGHT:	cout << "right turn" << endl;	break;
		}
	}
}
