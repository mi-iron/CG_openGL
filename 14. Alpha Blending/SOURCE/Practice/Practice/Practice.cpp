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
vec3	eye(0, 0, 3);
vec3	up(0, 1, 0);
vec3	center(0, 0, 0);

//light configuraion
vec4	light(0.0, 0.0, 8, 1);	//light position

//global coordinate frame
bool	axes = true;
float	AXIS_LENGTH = 1.25;
float	AXIS_LINE_WIDTH = 2;

//colors
GLfloat	bgColor[4] = { 1, 1, 1, 1 };

// control variable
int		selection = 1;	//selected example
bool	cube = false;
bool	depthMask = false;

//play configuration
bool	pause = true;

float timeStep = 1.0f / 120;
float period = 4.0;

// Current frame
int frame = 0;

//opengl texture unit
GLuint	texID[7];

int main(int argc, char* argv[])
{

	// vsync should be 0 for precise time stepping.
	vsync = 0;

	//filed of view of 35mm lens in degree
	fovy = 37.8f;

	// Initialize the OpenGL system
	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;


	// Callbacks
	glfwSetKeyCallback(window, keyboard);

	// Depth test
	glEnable(GL_DEPTH_TEST);

	// Normal vectors are normalized after transformation.
	glEnable(GL_NORMALIZE);

	// Viewport and perspective setting
	reshape(window, windowW, windowH);

	// Initialization - Main Loop - Finalization
	//
	init();

	// Main loop
	float previous = (float)glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Time passed during a single loop
		float now = (float)glfwGetTime();
		float delta = now - previous;
		previous = now;

		// Time passed after the previous frame
		elapsed += delta;

		// Deal with the current frame
		if (elapsed > timeStep)
		{
			// Animate 1 frame
			if (!pause) frame += 1;

			elapsed = 0;
		}

		render(window);
		glfwSwapBuffers(window);
	}

	// Terminate the glfw system
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// Texture
void loadCheckerboardTexture()
{

	GLubyte checkerboard[64][64][3];

	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
		{
			int c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;

			checkerboard[i][j][0] = (GLubyte)c;
			checkerboard[i][j][1] = (GLubyte)c;
			checkerboard[i][j][2] = (GLubyte)c;

		}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 64, 64, 0,
		GL_RGB, GL_UNSIGNED_BYTE, checkerboard);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

bool loadAlphaTexture(const char* filename, int w, int h)
{
	// Open the raw texture file
	ifstream is(filename, ios::binary);
	if (is.fail())
	{
		cout << "Can't open " << filename << endl;
		return false;
	}

	// Allocate memory
	GLubyte* raw = new GLubyte[w * h];

	// Read all the texels
	is.read((char*)raw, w * h);
	if (!is) cout << "Error: only " << is.gcount() << "bytes could be read!" << endl;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, raw);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Deallocate memory
	delete[] raw;

	return true;
}

void init()
{
	//gemerate 7 textures and bind appropriate images
	glGenTextures(7, texID);

	//texture
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadCheckerboardTexture();

	//raw texture
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glPixelTransferf(GL_RED_BIAS, 1.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[2]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 1.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[3]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 1.0);
	loadAlphaTexture("m02_logo.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[4]);
	glPixelTransferf(GL_RED_BIAS, 1.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[5]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 1.0);
	glPixelTransferf(GL_BLUE_BIAS, 0.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512);

	glBindTexture(GL_TEXTURE_2D, texID[6]);
	glPixelTransferf(GL_RED_BIAS, 0.0);
	glPixelTransferf(GL_GREEN_BIAS, 0.0);
	glPixelTransferf(GL_BLUE_BIAS, 1.0);
	loadAlphaTexture("m02_grayscale_ornament.raw", 512, 512);

	//prepare mesh
	readMesh("m01_bunny.off");

	//keyboard
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: x for axes on/off" << endl;
	cout << "Keyboard input: c for an opaque cube on/off" << endl;
	cout << "Keyboard input: d for depth mask on/off" << endl;
	cout << endl;
	cout << "Keyboard input: 1 for a flat transparent bunny" << endl;
	cout << "Keyboard input: 2 for a smooth transparent bunny" << endl;
	cout << "Keyboard input: 3 for a flat transparent bunny with depth sorting" << endl;
	cout << "Keyboard input: 4 for a smooth transparent bunny with depth sorting" << endl;
	cout << "Keyboard input: 5 for a alpha-textured cube" << endl;
}

void quit() {
	//delete mesh
	deleteMesh();
}

// Material
void setupColoredMaterial(const vec4& color)
{
	// Material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100;

	//transparency
	mat_ambient[3] *= color[3];
	mat_specular[3] *= color[3];

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(color));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawTexturedCube()
{
	vec3 normal[6];
	vec3 vertex[6][4];
	vec2 texcoord[4];

	// Front
	normal[0] = vec3(0, 0, 1);
	vertex[0][0] = vec3(0, 0, 1);
	vertex[0][1] = vec3(1, 0, 1);
	vertex[0][2] = vec3(1, 1, 1);
	vertex[0][3] = vec3(0, 1, 1);

	// Back
	normal[1] = vec3(0, 0, -1);
	vertex[1][0] = vec3(1, 0, 0);
	vertex[1][1] = vec3(0, 0, 0);
	vertex[1][2] = vec3(0, 1, 0);
	vertex[1][3] = vec3(1, 1, 0);

	// Right
	normal[3] = vec3(1, 0, 0);
	vertex[3][0] = vec3(1, 0, 1);
	vertex[3][1] = vec3(1, 0, 0);
	vertex[3][2] = vec3(1, 1, 0);
	vertex[3][3] = vec3(1, 1, 1);

	// Left
	normal[2] = vec3(-1, 0, 0);
	vertex[2][0] = vec3(0, 0, 0);
	vertex[2][1] = vec3(0, 0, 1);
	vertex[2][2] = vec3(0, 1, 1);
	vertex[2][3] = vec3(0, 1, 0);

	// Top
	normal[4] = vec3(0, 1, 0);
	vertex[4][0] = vec3(0, 1, 1);
	vertex[4][1] = vec3(1, 1, 1);
	vertex[4][2] = vec3(1, 1, 0);
	vertex[4][3] = vec3(0, 1, 0);

	// Bottom
	normal[5] = vec3(0, -1, 0);
	vertex[5][0] = vec3(0, 0, 0);
	vertex[5][1] = vec3(1, 0, 0);
	vertex[5][2] = vec3(1, 0, 1);
	vertex[5][3] = vec3(0, 0, 1);

	// Texture coordinate
	texcoord[0] = vec2(0, 1);
	texcoord[1] = vec2(1, 1);
	texcoord[2] = vec2(1, 0);
	texcoord[3] = vec2(0, 0);

	// Cube
	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texID[i + 1]);

		glBegin(GL_QUADS);
		glNormal3fv(value_ptr(normal[i]));
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2fv(value_ptr(texcoord[j]));
			glVertex3fv(value_ptr(vertex[i][j]));
		}
		glEnd();
	}
}

//read a mesh from a given OFF file
int		nVertices = 0, nFaces = 0, nEdges = 0;
glm::vec3* vertex = NULL;
glm::vec3* vnormal = NULL; //vetex normal
glm::vec3* fnormal = NULL; //face normal
int* face[3] = { NULL, NULL, NULL };


struct DepthSortData
{
	int		i;		//face index
	vec4	center;	//face center for depth sorting
	float	depth;	//depth from the eye
};
DepthSortData* fdsd = NULL;

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

	//depth sort data
	fdsd = new DepthSortData[nFaces];

	int n;
	vec3	center;
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

		//depth sorting data
		center = vertex[face[0][i]] / 3.0f;
		center += vertex[face[1][i]] / 3.0f;
		center += vertex[face[2][i]] / 3.0f;

		fdsd[i].i = i;
		fdsd[i].center = vec4(center, 1.0f);
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
	if (fdsd) { delete[] fdsd; fdsd = NULL; }
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

//draw a smooth mesh by specifying its vertex normal vectors
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

//decreasing order to draw the farther faces first
int compare(const void* a, const void* b)
{
	float	depth1 = ((DepthSortData*)a)->depth;
	float	depth2 = ((DepthSortData*)b)->depth;

	if (depth1 > depth2)		return -1;
	if (depth1 < depth2)		return 1;

	return 0;
}

void sortMeshFace()
{
	//get the current model view matrix
	GLfloat	modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	//opengl employs the row vector convention
	//and glm employs the column-major representation
	mat4	M = make_mat4(modelView);

	//depth of every face
	for (int i = 0; i < nFaces; i++)
	{
		//face center in the eye coordinate system
		vec4	center_eye = M * fdsd[i].center;

		//the camera faces the negative z-axis in opengl
		fdsd[i].depth = -center_eye.z;
	}

	//depth sorting
	qsort(fdsd, nFaces, sizeof(DepthSortData), compare);
}

//draw a flat mesh with depth sorting
void drawSortedFlatMesh()
{
	//sort mesh faces
	sortMeshFace();

	//geometry
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
	{
		int iFace = fdsd[i].i;

		glNormal3fv(value_ptr(fnormal[iFace]));
		for (int j = 0; j < 3; j++)
			glVertex3fv(value_ptr(vertex[face[j][iFace]]));
	}
	glEnd();
}

//draw a smooth mesh by specifying its vertex normal vectors
void drawSortedSmoothMesh() {
	//sort mesh faces
	sortMeshFace();

	//geometry
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < nFaces; i++)
		for (int j = 0; j < 3; j++) {
			int		iFace = fdsd[i].i;

			glNormal3fv(value_ptr(vnormal[face[j][iFace]]));
			glVertex3fv(value_ptr(vertex[face[j][iFace]]));
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

//textured quad
void drawTexturedQuad()
{
	glBindTexture(GL_TEXTURE_2D, texID[0]);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);

	glTexCoord2f(0, 0);	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);	glVertex3f(1, 0, 0);
	glTexCoord2f(1, 1);	glVertex3f(1, 1, 0);
	glTexCoord2f(0, 1);	glVertex3f(0, 1, 0);

	glEnd();
}

void render(GLFWwindow* window) {

	//bg color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);

	//enabling writing into the depth buffer
	glDepthMask(true);

	//clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	//axes
	if (axes) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	//smooth shading
	glShadeModel(GL_SMOOTH);

	//lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	setupLight(light);

	//draw opaque object first
	//
	glDepthMask(true);		//enabling writing into the depth buffer

	//solid objects do not require two-sided lighting
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	//textured opaque quad
	//
	glPushMatrix();
	{
		glScalef(2.5, 2.5, 1.0);
		glTranslatef(-0.5, -0.5, -1);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 1));
		drawTexturedQuad();
	}
	glPopMatrix();

	//opaque cube by turning off alpha texturing
	if (cube)
	{
		glPushMatrix();

		glTranslatef(-0.15f, 0.3f, 0.5f);
		glScalef(0.3f, 0.3f, 0.3f);

		vec3	axis(1, 0, 0);
		glRotatef(30, axis.x, axis.y, axis.z);

		glDisable(GL_TEXTURE_2D);

		setupColoredMaterial(vec4(0.5, 0.95, 0.5, 1));
		drawTexturedCube();

		glPopMatrix();

	}

	//draw transparent ones
	//
	glDepthMask(depthMask);		//disabling writing into the depth buffer

	//back faces should be shaded in transparent objects
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if (selection <= 4)
	{
		glPushMatrix();

		float	theta = frame * 4 / period;
		vec3	axis(0, 1, 0);

		glRotatef(theta, axis.x, axis.y, axis.z);

		glTranslatef(0.0f, -0.2f, 0.0f);
		glScalef(0.7f, 0.7f, 0.7f);

		//non-textured but transparent
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 0.5));
		switch (selection)
		{
		case 1: drawFlatMesh();				break;
		case 2: drawSmoothMesh();			break;
		case 3: drawSortedFlatMesh();		break;
		case 4: drawSortedSmoothMesh();		break;
		}

		glPopMatrix();
	}
	else {		//transparent cube
		glPushMatrix();

		//rotating
		float	theta = frame * 4 / period;
		vec3	axis(1, 1, 0);
		glRotatef(theta, axis.x, axis.y, axis.z);

		//centering
		glTranslatef(-0.3f, -0.3f, -0.3f);
		glScalef(0.6f, 0.6f, 0.6f);

		//alpha-textured
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//draw
		setupColoredMaterial(vec4(0.95, 0.95, 0.95, 1));
		drawTexturedCube();

		glPopMatrix();
	}
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE:glfwSetWindowShouldClose(window, GL_TRUE); break;

			//Play on/off
		case GLFW_KEY_SPACE: pause = !pause; break;

			// Axes on/off
		case GLFW_KEY_X: axes = !axes; break;

			//opaque cube on,off
		case GLFW_KEY_C: cube = !cube; break;

			//depth mask on, off
		case GLFW_KEY_D: depthMask = !depthMask; break;

			//example selection
		case GLFW_KEY_1:	selection = 1; break;
		case GLFW_KEY_2:	selection = 2; break;
		case GLFW_KEY_3:	selection = 3; break;
		case GLFW_KEY_4:	selection = 4; break;
		case GLFW_KEY_5:	selection = 5; break;

		}
	}


}
