#include "glSetup.h"

#ifdef _WIN32
#define	_USE_MATH_DEFINES
#endif // _WIN32

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

bool	exponent = false;
float	exponentInitial = 0.0; //[0, 128]
float	exponentValue = exponentInitial;
float	exponentNorm = exponentValue / 128.0f; // [0, 1]

bool	cutoff = false;
float	cutoffMax = 60;					//[0, 90] degree
float	cutoffInitial = 30.0;			//[0, cutoffMax] degree
float	cutoffValue = cutoffInitial;	
float	cutoffNorm = cutoffValue / cutoffMax;	//[0, 1]

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

	exponentValue = exponentInitial;
	exponentNorm = exponentValue / 128.0f;

	cutoffValue = cutoffInitial;
	cutoffNorm = cutoffValue / cutoffMax;

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

	//periodically change the exponent and/or cutoff value of the spot light
	if (lightOn[2] && exponent) {
		exponentNorm += float(radians(4.0 / period) / M_PI);
		exponentValue = float(128.0f * (acos(cos(exponentNorm * M_PI)) / M_PI));
	}
	if (lightOn[2] && cutoff) {
		cutoffNorm += float(radians(4.0 / period) / M_PI);
		cutoffValue = float(cutoffMax * (acos(cos(cutoffNorm * M_PI)) / M_PI));
	}
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
GLUquadricObj* cylinder = NULL;
GLUquadricObj* cone = NULL;
void init() {
	//animation system
	reinitialize();

	//prepare quadric shapes
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricTexture(sphere, GLU_FALSE);

	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricOrientation(cylinder, GLU_OUTSIDE);
	gluQuadricTexture(cylinder, GLU_FALSE);

	cone = gluNewQuadric();
	gluQuadricDrawStyle(cone, GLU_FILL);
	gluQuadricNormals(cone, GLU_SMOOTH);
	gluQuadricOrientation(cone, GLU_OUTSIDE);
	gluQuadricTexture(cone, GLU_FALSE);

	readMesh("m01_bunny.off");

	//keyboard
	cout << endl;
	cout << "Keyboard Input : space for play/pause" << endl;
	cout << "Keyboard Input : i for reinitialization" << endl;
	cout << "Keyboard Input : up for increasing period" << endl;
	cout << "Keyboard Input : down for decreasing period" << endl;
	cout << "Keyboard Input : v for top, front, bird-eye view" << endl;
	cout << endl;
	cout << "Keyboard Input : p for a point light" << endl;
	cout << "Keyboard Input : d for a distant light" << endl;
	cout << "Keyboard Input : s for a spot light" << endl;
	cout << "Keyboard Input : a for light attenuation" << endl;
	cout << "Keyboard Input : e for time dependent exponent of a spot light" << endl;
	cout << "Keyboard Input : c for time dependent cutoff of a spot light" << endl;
	cout << "Keyboard Input : m for predefined material parameters" << endl;
	cout << "Keyboard Input : l for rotation of lights" << endl;
	cout << "Keyboard Input : o for rotation of objects" << endl;
	cout << "Keyboard Input : 1 for 3x3 spheres" << endl;
	cout << "Keyboard Input : 2 for 3x3 flat bunnies" << endl;
	cout << "Keyboard Input : 3 for 3x3 smooth bunnies" << endl;
	cout << "Keyboard Input : 4 for a plane with many small squres" << endl;
}

void quit() {
	//delete quadric shapes
	//delete quadric shapes
	gluDeleteQuadric(sphere);
	gluDeleteQuadric(cylinder);
	gluDeleteQuadric(cone);

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

void setupWhiteShinyMaterial() {
	//material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess = 100;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void setupBrassMaterial() {
	//material
	GLfloat mat_ambient[4] = { 0.33f, 0.22f, 0.33f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.78f, 0.57f, 0.11f, 1.0f };
	GLfloat mat_specular[4] = { 0.99f, 0.91f, 0.81f, 1.0f };
	GLfloat mat_shininess = 27.8f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void setupRedPlasticMaterial() {
	//material
	GLfloat mat_ambient[4] = { 0.3f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.6f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_specular[4] = { 0.8f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_shininess = 32.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawSquare() {
	//square
	glBegin(GL_QUADS);

	//front
	glNormal3f(0, 0, 1);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(0, 1, 1);

	glEnd();
}

//draw objects using a GLU quadric
void drawSphere(float radius, int slices, int stacks) {

	gluSphere(sphere, radius, slices, stacks);
}

void drawCylinder(float radius, float height, int slices, int stacks) {

	gluCylinder(cylinder, radius, radius, height, slices, stacks);
}

void drawCone(float radius, float height, int slices, int stacks) {

	gluCylinder(cone, 0, radius, height, slices, stacks);
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
	
	glLightfv(GL_LIGHT0+i, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0+i, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0+i, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0+i, GL_POSITION, value_ptr(p));

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

	if (i == 2)
	{
		vec3		spotDirection = -vec3(p);
		glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, value_ptr(spotDirection));
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, cutoffValue); //[0, 90]
		glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, exponentValue); //[0, 128]
	}
	else
	{
		//point and distant light
		//180 to turn off cutoff when it has been used as a spot light
		glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180); //uniform light distribution
	}
}

void drawArrow(const vec3& p, bool tailOnly) {
	//make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//common material
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 25;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

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

	if (!tailOnly) {
		//shaft cylinder
		float	arrowShaftRadius = 0.02f;
		float	arrowShaftLength = 0.2f;
		glColor3f(0, 1, 0);
		drawCylinder(arrowShaftRadius, arrowShaftLength, 16, 5);

		//head cone
		float	arrowheadHeight = 0.09f;
		float	arrowheadRadius = 0.06f;
		glTranslatef(0, 0, arrowShaftLength + arrowheadHeight);
		glRotatef(180, 1, 0, 0);
		glColor3f(0, 0, 1);
		drawCone(arrowheadRadius, arrowheadHeight, 16, 5);
	}

	glPopMatrix();

	//for convential material setting
	glDisable(GL_COLOR_MATERIAL);
}

void drawSpotLight(const vec3& p, float cutoff) {
	glPushMatrix();

	glTranslatef(p.x, p.y, p.z);

	float	theta;
	vec3	axis;
	computeRotation(vec3(0, 0, 1), vec3(0, 0, 0) - vec3(p), theta, axis);
	glRotatef(theta, axis.x, axis.y, axis.z);

	//color
	setupColoredMaterial(vec3(0, 0, 1));

	// tan(cutoff) = r/h
	float	h = 0.15f;
	float	r = h * tan(radians(cutoff));
	drawCone(r, h, 16, 5);

	//color
	setupColoredMaterial(vec3(1, 0, 0));

	//apex
	float	apexRadius = 0.06f * (0.5f + exponentValue / 128.0f);
	drawSphere(apexRadius, 16, 16);

	glPopMatrix();
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

		if (i == 2)	drawSpotLight(lightP[i], cutoffValue);
		else drawArrow(lightP[i], i == 0);
	}

	//draw objects
	switch (material)
	{
	case 0: setupWhiteShinyMaterial();	break;
	case 1: setupBrassMaterial();		break;
	case 2: setupRedPlasticMaterial();	break;
	}

	if (selection == 4)
	{
		float	width = 0.05f;
		float	height = 0.05f;
		float	depth = 0.1f;
		for(float i= -2.2f; i<=2.2f; i += width)
			for (float j = -1.3f; j <= 1.3f; j += height) {
				glPushMatrix();

				glTranslatef(i, j, -1.0);
				glScalef(width, height, depth);
				glTranslatef(-0.5, -0.5, -0.5);
				drawSquare();

				glPopMatrix();
			}
	}
	else {
		glScalef(0.4f, 0.4f, 0.4f);

		vec3	u[3];
		vec3	v[3];
		u[0] = vec3(0, 1, 0) * 2.0f;	u[1] = vec3(0, 0, 0);	u[2] = -u[0];
		v[0] = -vec3(1, 0, 0) * 4.0f;	v[1] = vec3(0, 0, 0);	v[2] = -v[0];

		for (int i = 0; i < 3; i++) {
			glPushMatrix();
			glTranslatef(u[i].x, u[i].y, u[i].z);

			for (int j = 0; j < 3; j++) {
				glPushMatrix();

				glTranslatef(v[j].x, v[j].y, v[j].z);
				glRotatef(thetaModel, axis.x, axis.y, axis.z);

				switch (selection) {
				case 1: drawSphere(0.7f, 64, 64);	break;
				case 2: drawFlatMesh();				break;
				case 3: drawSmoothMesh();			break;
				}

				glPopMatrix();
			}
			glPopMatrix();
		}
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
		case GLFW_KEY_D: lightOn[1] = !lightOn[1];	break;
		case GLFW_KEY_S: lightOn[2] = !lightOn[2];	break;

		case GLFW_KEY_A: attenuation = !attenuation;		break;
		case GLFW_KEY_L: rotationLight = !rotationLight;	break;
		case GLFW_KEY_O: rotationObject = !rotationObject;	break;
		case GLFW_KEY_E: exponent = !exponent;				break;
		case GLFW_KEY_C: cutoff = !cutoff;					break;
		case GLFW_KEY_M: material = (material+1)%3;			break;

			// example selection
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		case GLFW_KEY_3: selection = 3; break;
		case GLFW_KEY_4: selection = 4; break;

		}
	}
}