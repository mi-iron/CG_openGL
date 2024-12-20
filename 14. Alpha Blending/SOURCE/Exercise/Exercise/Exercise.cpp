#include "glSetup.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void init();
void quit();
void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);

void deleteMesh();

// Camera configuration
vec3 eye(0, 0, 3);
vec3 up(0, 1, 0);
vec3 center(0, 0, 0);

// Light configuration
vec4 light(0.0, 0.0, 8, 1); // Light position

// Global coordinate frame
bool axes = true;
float AXIS_LENGTH = 1.25;
float AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1,1,1,1 };

// control variable
int selection = 1;
bool cube = false;
bool depthMask = false;

// Play Configuration
bool pause = true;

float timeStep = 1.0f / 120;
float period = 4.0;

// Current frame
int frame = 0;

// OpenGL texture unit
GLuint texID[7];

int main(int argc, char* argv[])
{
	// vsync should be 0 for precise time stepping.
	vsync = 0;

	// Field of view of 35mm lens in degree
	fovy = 37.8f;

	// Initialize the OpenGl system
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


	// Initialization - Main loop - Finalization
	//
	init();

	// Main loop
	float previous = (float)glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); //Events

		// Time passed during a single loop
		float now = (float)glfwGetTime();
		float delta = now - previous;
		previous = now;

		// Tieme pased after the previous frame
		elapsed += delta;

		// Deal with the current frame
		if (elapsed > timeStep)
		{
			// Animate 1 frame
			if (!pause) frame += 1;

			elapsed = 0; // Reset the elapsed time
		}

		render(window); // Draw one frame
		glfwSwapBuffers(window); // Swap buffers;
	}

	//Finalization
	quit();

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

	delete[] raw;

	return true;
}

void init()
{
	// Generate 7 textures and bind appropriate images
	glGenTextures(7, texID);

	// Txture
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	loadCheckerboardTexture();

	// Raw texture
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


	// Keyboard
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: x for axes on/off" << endl;
	cout << "Keyboard input: c for an opaque cube on/off" << endl;
	cout << "Keyboard input: d for depth mask on/off" << endl;
	cout << endl;
	cout << "Keyboard input: 1 for a alpha-textured cube" << endl;
	cout << "Keyboard input: 2 for a alpha-textured cube with depth sorting" << endl;
}

void quit()
{
	// Delete mesh
	deleteMesh();
}

// Material
void setupColoredMaterial(const vec4& color)
{
	// Material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 100;

	// Transparency
	mat_ambient[3] *= color[3];
	mat_specular[3] *= color[3];

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(color));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}
struct DepthSortData
{
	int i;
	vec4 center;
	float depth;
};
DepthSortData* fdsd = NULL;
int compare(const void* a, const void* b)
{
	float depth1 = ((DepthSortData*)a)->depth;
	float depth2 = ((DepthSortData*)b)->depth;

	if (depth1 > depth2)return -1;
	if (depth1 < depth2)return 1;

	return 0;
}

void sortCubeFace()
{
	// Get the current model view matrix
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	// OpenGL employs the row vector convention
	// and glm employs the column-major representation.
	mat4 M = make_mat4(modelView);

	// Depth of every face
	for (int i = 0; i < 6; i++)
	{
		// Face center in the eye coordinate system
		vec4 center_eye = M * fdsd[i].center;

		// The camera faces the negative z-axis in OpenGL
		fdsd[i].depth = -center_eye.z;
	}
	// Depth sorting
	qsort(fdsd, 6, sizeof(DepthSortData), compare);
}

void drawSortedTexturedCube()
{
	vec3 normal[6];
	vec3 vertex[6][4];
	vec2 texcoord[4];
	vec3 center;
	fdsd = new DepthSortData[6];

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

	for (int i = 0; i < 6; i++)
	{
		center = vec3(0, 0, 0);
		for (int j = 0; j < 4; j++)
			center += vertex[i][j] / 4.0f;
		fdsd[i].i = i;
		fdsd[i].center = vec4(center, 1.0f);
	}

	// Texture coordinate
	texcoord[0] = vec2(0, 1);
	texcoord[1] = vec2(1, 1);
	texcoord[2] = vec2(1, 0);
	texcoord[3] = vec2(0, 0);


	// Sort cube faces
	sortCubeFace();
	// Cube
	for (int i = 0; i < 6; i++)
	{
		int iFace = fdsd[i].i;
		glBindTexture(GL_TEXTURE_2D, texID[iFace + 1]);

		glBegin(GL_QUADS);
		glNormal3fv(value_ptr(normal[iFace]));
		for (int j = 0; j < 4; j++)
		{
			glTexCoord2fv(value_ptr(texcoord[j]));
			glVertex3fv(value_ptr(vertex[iFace][j]));
		}
		glEnd();
	}
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

// Delete the vertices and faces of the mesh
void deleteMesh()
{
	if (fdsd) { delete[] fdsd; fdsd = NULL; }
}


// Light
void setupLight(const vec4& p)
{
	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f,1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(p));
}

// Textured quad
void drawTexturedQuad()
{
	glBindTexture(GL_TEXTURE_2D, texID[0]);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);

	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
	glTexCoord2f(0, 1); glVertex3f(0, 1, 0);

	glEnd();
}

void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);

	// Enabling writing into the depth buffer
	glDepthMask(true);

	// Clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

	// Axes
	if (axes)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	// Smooth shading
	glShadeModel(GL_SMOOTH);


	// Lighting
	//
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	setupLight(light);

	// Draw opaque object first
	//
	glDepthMask(true);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

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

	// Opaque cube by turning off alpha textureing
	if (cube)
	{
		glPushMatrix();

		glTranslatef(-0.15f, 0.3f, 0.5f);
		glScalef(0.3f, 0.3f, 0.3f);

		vec3 axis(1, 0, 0);
		glRotatef(30, axis.x, axis.y, axis.z);

		glDisable(GL_TEXTURE_2D);

		setupColoredMaterial(vec4(0.5, 0.95, 0.5, 1));
		drawTexturedCube();

		glPopMatrix();
	}

	// Draw transparent ones
	//
	glDepthMask(depthMask);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


	glPushMatrix();

	// Rotating
	float theta = frame * 4 / period;
	vec3 axis(1, 1, 0);
	glRotatef(theta, axis.x, axis.y, axis.z);

	// Centering
	glTranslatef(-0.3f, -0.3f, -0.3f);
	glScalef(0.6f, 0.6f, 0.6f);

	// Alpha-textured
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw
	setupColoredMaterial(vec4(0.95, 0.95, 0.95, 1));
	if (selection == 1) drawTexturedCube();
	else if (selection == 2) drawSortedTexturedCube();

	glPopMatrix();

}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
			//Quit
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

			//Axes
		case GLFW_KEY_X:axes = !axes; break;
			// Play on/off
		case GLFW_KEY_SPACE: pause = !pause; break;
			// Opaque cube on/off
		case GLFW_KEY_C:cube = !cube; break;
			// Polygon fill on/off
		case GLFW_KEY_D:depthMask = !depthMask; break;

			// Example section
		case GLFW_KEY_1: selection = 1; break;
		case GLFW_KEY_2: selection = 2; break;
		}
	}
}