#include "glSetup.h"
#include <iostream>

void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);

GLfloat bgColor[4] = { 1,1,1,1 };

GLenum mode = GL_QUADS;
bool fill = true;

int
main(int argc, char* argv[])
{
	perspectiveView = false;

	GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
	if (window == NULL) return -1;

	glfwSetKeyCallback(window, keyboard);

	reshape(window, windowW, windowH);

	std::cout << std::endl;
	std::cout << "Keyboard Input: 1 GL_QUAD" << std::endl;
	std::cout << "Keyboard Input: 2 GL_QUAD_STRIP" << std::endl;
	std::cout << std::endl;
	std::cout << "Keyboard Input: f for polygon filling on/off" << std::endl;

	while (!glfwWindowShouldClose(window)) {
		render(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
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

	if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPointSize(5 * dpiScaling);
	glLineWidth(5 * dpiScaling);

	if (mode == GL_QUADS) {
		glBegin(mode);
		{
			glColor3f(1, 0, 0); glVertex2f(-0.2, -0.5);
			glColor3f(1, 1, 0); glVertex2f(0.2, -0.5);
			glColor3f(0, 1, 0); glVertex2f(0.5, -0.2);
			glColor3f(0, 1, 1); glVertex2f(0.5, 0.2);

			glColor3f(0, 0, 1); glVertex2f(0.2, 0.5);
			glColor3f(0, 0, 1); glVertex2f(-0.2, 0.5);
			glColor3f(0, 0, 0); glVertex2f(-0.5, 0.2);
			glColor3f(0, 0, 0); glVertex2f(-0.5, -0.2);
		}
		glEnd();
	}
	else if (mode == GL_QUAD_STRIP) {
		glBegin(mode);
		{
			glColor3f(1, 0, 0); glVertex2f(-0.2, -0.5);
			glColor3f(1, 1, 0); glVertex2f(0.2, -0.5);
			glColor3f(0, 1, 1); glVertex2f(0.5, 0.2);
			glColor3f(0, 1, 0); glVertex2f(0.5, -0.2);

			glColor3f(0, 0, 1); glVertex2f(0.2, 0.5);
			glColor3f(0, 0, 1); glVertex2f(-0.2, 0.5);

			glColor3f(0, 0, 0); glVertex2f(-0.5, -0.2);
			glColor3f(0, 0, 0); glVertex2f(-0.5, 0.2);
		}
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

		case GLFW_KEY_F: fill = !fill; break;

		case GLFW_KEY_1: mode = GL_QUADS; break;
		case GLFW_KEY_2: mode = GL_QUAD_STRIP;  break;
		}
	}
}