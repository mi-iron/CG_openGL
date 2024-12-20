#include "glSetup.h"
#include <iostream>

void render(GLFWwindow* window);
void keyboard(GLFWwindow* window, int key, int code, int action, int mods);

GLfloat bgColor[4] = { 1,1,1,1 };

GLenum mode = GL_POINTS;
bool stipple = false;
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
	std::cout << "Keyboard Input: 1 GL_POINTS" << std::endl;
	std::cout << "Keyboard Input: 2 GL_LINES" << std::endl;
	std::cout << "Keyboard Input: 3 GL_LINE_STRIP" << std::endl;
	std::cout << "Keyboard Input: 4 GL_LINE_LOOP" << std::endl;
	std::cout << "Keyboard Input: 5 GL_TRIANGLES" << std::endl;
	std::cout << "Keyboard Input: 6 GL_TRIANGLE_STRIP" << std::endl;
	std::cout << "Keyboard Input: 7 GL_TRIANGLE_FAN" << std::endl;
	std::cout << "Keyboard Input: 8 GL_POLYGON" << std::endl;
	std::cout << std::endl;
	std::cout << "Keyboard Input: s for stippling on/off" << std::endl;
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

	if (stipple) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(int(3 * dpiScaling), 0xcccc);
	}
	else glDisable(GL_LINE_STIPPLE);

	if (fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPointSize(5 * dpiScaling);
	glLineWidth(5 * dpiScaling);

	glBegin(mode);
	{
		glColor3f(1, 0, 0); glVertex2f(-0.5, -0.5);
		glColor3f(0, 1, 0); glVertex2f(0.5, -0.5);
		glColor3f(0, 0, 1); glVertex2f(0.5, 0.5);
		glColor3f(0, 0, 0); glVertex2f(-0.5, 0.5);
	}
	glEnd();
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

		case GLFW_KEY_S: stipple = !stipple; break;
		case GLFW_KEY_F: fill = !fill; break;

		case GLFW_KEY_1: mode = GL_POINTS; break;
		case GLFW_KEY_2: mode = GL_LINES;  break;
		case GLFW_KEY_3: mode = GL_LINE_STRIP;  break;
		case GLFW_KEY_4: mode = GL_LINE_LOOP;  break;
		case GLFW_KEY_5: mode = GL_TRIANGLES;  break;
		case GLFW_KEY_6: mode = GL_TRIANGLE_STRIP; break;
		case GLFW_KEY_7: mode = GL_TRIANGLE_FAN; break;
		case GLFW_KEY_8: mode = GL_POLYGON; break;

		case GLFW_KEY_SPACE: break;
		case GLFW_KEY_PERIOD: break;
		case GLFW_KEY_LEFT: break;
		case GLFW_KEY_RIGHT: break;
		case GLFW_KEY_LEFT_BRACKET: break;
		case GLFW_KEY_RIGHT_BRACKET: break;
		case GLFW_KEY_UP: break;
		case GLFW_KEY_DOWN: break;
		}
	}
}