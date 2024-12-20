#include "glSetup.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
using namespace std;

void init();
void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);

// Camera configuration
vec3 eye1(0, 0, 3);
vec3 at1(0, 0, 0);
vec3 upD(0, 1, 0);
vec3 forwardD;

// Light Configuration
vec4 light1(0.0, 0.0, 1.0, 0);

// Global coordinate frame
bool axes = true;
float AXIS_LENGTH = 2;
float AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1,1,1,1 };

// Play Configuration
bool pause = true;

float timeStep = 1.0f / 120;
float period = 4.0;

// Current frame
int frame = 0;

//selection
int selection = 1;

// Textured square, upside down
bool upsideDown = true;


// OpenGL texture unit
GLuint texID[4];

// Texture parameters
bool texture = true;
float textureNumRepeats = 1;

// MSAA
bool aaEnabled = false;

int main(int argc, char* argv[])
{
	// Setup far plane to move back for miamapping test
	farDist = 200.0;

	// vsync should be 0 for precise time stepping.
	vsync = 0;

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

bool loadRawTexture(const char* filename, int w, int h, int n)
{
	// Open the raw texture file
	ifstream is(filename, ios::binary);
	if (is.fail())
	{
		cout << "Can't open " << filename << endl;
		return false;
	}

	// Allocate memory
	GLubyte* raw = new GLubyte[w * h * n];

	// Read all the texels
	is.read((char*)raw, w * h * n);
	if (!is) cout << "Error: only " << is.gcount() << "bytes could be read!" << endl;

	// Only 3 and 1

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0,
		GL_RGB, GL_UNSIGNED_BYTE, raw);


	switch (selection)
	{
	case 1:	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	case 2:	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); break;
	case 3:	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	case 4:	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); break;
	case 5:	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Deallocate memory
	delete[] raw;

	return true;
}


void init()
{

	// Generate 4 textures and bind appropriate images
	glGenTextures(4, texID);

	// Raw texture
	glBindTexture(GL_TEXTURE_2D, texID[2]);
	loadRawTexture("m02_marble.raw", 512, 512, 3);

	// Keyboard
	cout << endl;
	cout << "Keyboard input: space for play/pause" << endl;
	cout << "Keyboard input: x for axes on/off" << endl;
	cout << endl;
	cout << "Keyboard input: 1 for repeat 1" << endl;
	cout << "Keyboard input: 2 for repeat 2, s clamp, t clamp" << endl;
	cout << "Keyboard input: 3 for repeat 2, s clamp, t repeat" << endl;
	cout << "Keyboard input: 4 for repeat 2, s repeat, t clamp" << endl;
	cout << "Keyboard input: 5 for repeat 2, s repeat, t repeat" << endl;


}

// Light
void setupLight(const vec4& light)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));
}

// Material
void setupMaterial()
{
	// Make it possible to change a subset of material parameters
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Material
	GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_shininess = 25;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void drawTexturedSquare(float m = 1)
{
	// Square
	glBegin(GL_QUADS);

	// Object's color affects the texture because default color blending is GL_MODULATE.
	// That is, glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);

	// Upside down
	if (upsideDown)
	{
		glTexCoord2f(0, m - 0); glVertex3f(0, 0, 0);
		glTexCoord2f(m, m - 0); glVertex3f(1, 0, 0);
		glTexCoord2f(m, m - m); glVertex3f(1, 1, 0);
		glTexCoord2f(0, m - m); glVertex3f(0, 1, 0);
	}

	glEnd();
}



void render(GLFWwindow* window)
{
	// Background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Eye
	vec3 eye(0.0);
	vec3 at(0.0);
	eye = eye1; at = at1;
	gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], upD[0], upD[1], upD[2]);

	// Axes
	if (axes)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);
	}

	// Lighting
	vec4 light(0.0);
	light = light1;
	setupLight(light);

	// Material
	setupMaterial();

	// Texture on/off
	if (texture) glEnable(GL_TEXTURE_2D);
	else glDisable(GL_TEXTURE_2D);

	// Draw
	glBindTexture(GL_TEXTURE_2D, texID[2]);
	drawTexturedSquare(textureNumRepeats);
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
			// Texture filter
		case GLFW_KEY_1:selection = 1;	textureNumRepeats = 1; loadRawTexture("m02_marble.raw", 512, 512, 3); break;
		case GLFW_KEY_2:selection = 2;  textureNumRepeats = 2; loadRawTexture("m02_marble.raw", 512, 512, 3); break;
		case GLFW_KEY_3:selection = 3;	textureNumRepeats = 2; loadRawTexture("m02_marble.raw", 512, 512, 3); break;
		case GLFW_KEY_4:selection = 4;  textureNumRepeats = 2; loadRawTexture("m02_marble.raw", 512, 512, 3); break;
		case GLFW_KEY_5:selection = 5;  textureNumRepeats = 2; loadRawTexture("m02_marble.raw", 512, 512, 3); break;
		}
	}

	// With the shift key
	if ((action == GLFW_PRESS || action == GLFW_REPEAT) && (mods & GLFW_MOD_SHIFT))
	{

		switch (key)
		{
			// Texture number of repeats
		case GLFW_KEY_UP: textureNumRepeats += 1; break;
		case GLFW_KEY_DOWN:textureNumRepeats -= 1; break;
		}
	}
}