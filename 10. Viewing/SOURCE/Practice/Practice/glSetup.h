#pragma once
#pragma comment(lib, "lib/glfw3.lib")


#ifndef __GL_SETUP_H_
#define __GL_SETUP_H_
#include <Windows.h> 
#include <GL/glu.h> 
#include <GLFW/glfw3.h>

extern float	screenScalel;
extern int		screenW, screenH;
extern int		windowW, windowH;
extern float	aspect;
extern float	dpiScaling;

extern int		vsync;

extern bool		perspectiveView;
extern float	fovy;
extern float	nearDits;
extern float	farDist;

GLFWwindow* initializeOpenGL(int argc, char* argv[], GLfloat bg[4]);
void		reshape(GLFWwindow* window, int w, int n);
void		setupProjectionMatrix();

void		drawAxes(float l, float w);

#endif	// __GL_SETUP_H_#pragma once
#pragma once