#include "glSetup.h"
#include <iostream>
using namespace::std;

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <math.h>

void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);
void mouseButton(GLFWwindow* window, int button, int action, int mods);
void mouseMove(GLFWwindow* window, double x, double y);

float AXIS_LENGTH = 0.5;
float AXIS_LINE_WIDTH = 2.0;

GLfloat bgColor[4] = { 1,1,1,1 };

float point[2][2] = { {0,0},{0,0} };

enum class InputMode {
	NONE = 0,
	DRAGGING = 1,
	COMPLETE = 2,
};

InputMode inputMode = InputMode::NONE;

bool pause = true;
float timeStep = 1.0f / 120;
float period = 8.0;

void
rotate(float p[2], float delta_theta)
{
	float x = p[0];
	float y = p[1];

	float cos_delta_theta = cos(delta_theta);
	float sin_delta_theta = sin(delta_theta);

	p[0] = cos_delta_theta * x - sin_delta_theta * y;
	p[1] = sin_delta_theta * x + cos_delta_theta * y;
}

int
main(int argc, char* argv[])
{
	perspectiveView = false;

	vsync = 0;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetCursorPosCallback(window, mouseMove);

	reshape(window, windowW, windowH);

	cout << endl;
	cout << "Mouse button down: start point of the line segment" << endl;
	cout << "Mouse dragging:    changes the line segment" << endl;
	cout << "Mouse button up:   end point of the line segment" << endl;
	cout << "Keyboard input:    space for play/pause" << endl;
	cout << "Keyboard input:    up for increasing period" << endl;
	cout << "Keyboard input:    down for decreasing period" << endl;

	float previous = (float)glfwGetTime();
	float elapsed = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		float now = (float)glfwGetTime();
		float delta_t = now - previous;
		previous = now;

		elapsed += delta_t;

		if (elapsed > timeStep)
		{
			if (!pause && inputMode == InputMode::COMPLETE)
			{
				float delta_theta = float(2.0 * M_PI) / period * elapsed;

				rotate(point[0], delta_theta);
				rotate(point[1], delta_theta);
			}

			elapsed = 0;
		}

		render(window);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void
render(GLFWwindow* window)
{
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LINE_STIPPLE);
	drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

	glColor3f(0, 0, 1);
	glLineWidth(5 * dpiScaling);

	if (inputMode == InputMode::DRAGGING)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);
	}
	else glDisable(GL_LINE_STIPPLE);

	if (inputMode > InputMode::NONE)
	{
		glBegin(GL_LINES);
		glVertex2f(point[0][0], point[0][1]);
		glVertex2f(point[1][0], point[1][1]);
		glEnd();
	}
}

void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_Q:
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;

		case GLFW_KEY_SPACE: pause = !pause; break;

		case GLFW_KEY_UP: period += 0.1f; break;
		case GLFW_KEY_DOWN: period = (float)max(period - 0.1, 0.1); break;
		}
	}
}

void
screen2world(float xs, float ys, float& xw, float& yw)
{
	float aspect = (float)screenW / screenH;
	xw = 2.0f * (xs / (screenW - 1) - 0.5f) * aspect;
	yw = -2.0f * (ys / (screenH - 1) - 0.5f);
}

void
mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	double xs, ys;
	glfwGetCursorPos(window, &xs, &ys);

	float xw, yw;
	screen2world((float)xs, (float)ys, xw, yw);

	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputMode = InputMode::DRAGGING;

		point[0][0] = xw; point[0][1] = yw;
		point[1][0] = xw; point[1][1] = yw;
	}

	if (action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputMode = InputMode::COMPLETE;

		point[1][0] = xw, point[1][1] = yw;
	}
}

void
mouseMove(GLFWwindow* window, double x, double y)
{
	if (inputMode == InputMode::DRAGGING)
	{
		screen2world((float)x, (float)y, point[1][0], point[1][1]);
	}
}