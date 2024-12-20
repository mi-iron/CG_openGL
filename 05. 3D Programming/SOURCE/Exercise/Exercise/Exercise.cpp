#include "glSetup.h"

#ifdef _WIN32
#define _USE_MATH_DEFINES    // To include the definition of M_PI in math.h
#endif

#include <glm/glm.hpp>           // OpenGL Mathematics
#include <glm/gtc/type_ptr.hpp>  // value_ptr()
using namespace glm;

#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>

using namespace std;

void    init();
void    quit();
void    render(GLFWwindow* window);
void    keyboard(GLFWwindow* window, int key, int code, int action, int mods);

bool    readMesh(const char* filename);
void    deleteMesh();

// Camera configuation
vec3    eye(3, 3, 3);
vec3    center(0, 0, 0);
vec3    up(0, 0, 1);

// Light configuration
vec4    light(5.0, 5.0, 0.0, 1);    // Light position

// Global coordinate frame
float   AXIS_LENGTH = 1.75;
float   AXIS_LINE_WIDTH = 2;

// Colors
GLfloat bgColor[4] = { 1, 1, 1, 1 };

int
main(int argc, char* argv[])
{
    // FPS control: vsync should be 0 for precise time stepping
    vsync = 0;   // 0 for immediate mode (Tearing possible)

    // Initialize the OpenGL system
    GLFWwindow* window = initializeOpenGL(argc, argv, bgColor);
    if (window == NULL) return -1;

    // Callbacks
    glfwSetKeyCallback(window, keyboard);

    // Normal vectors are normalized after transformation.
    glEnable(GL_NORMALIZE);

    // Turn off back face culling for cylinder
    glDisable(GL_CULL_FACE);

    // Viewport and perspective setting
    reshape(window, windowW, windowH);

    // Initialization - Main loop - Finalization
    //
    init();

    // Main loop
    float   previous = (float)glfwGetTime();
    float   elapsed = 0;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();         // Events

        render(window);           // Draw one frame
        glfwSwapBuffers(window);  // Swap buffers
    }

    // Finalization
    quit();

    // Terminate the glfw system
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void
init()
{
    // Prepare mesh
    readMesh("m01_bunny.off");
    cout << endl;
}

void
quit()
{
    // Delete mesh
    deleteMesh();
}

// Light
void
setupLight()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, value_ptr(light));

    // For disk
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

// Material
void
setupMaterial()
{
    // Material
    GLfloat mat_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_specular[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess = 128;

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void
setDiffuseColor(const vec3& color)
{
    GLfloat mat_diffuse[4] = { color[0], color[1], color[2], 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
}

// Draw a sphere after setting up its material

// Read a mesh from a given OFF file
int     nVertices = 0, nFaces = 0, nEdges = 0;
vec3* vertex = NULL;
int* face[3] = { NULL, NULL, NULL };
set<set<int>> edges;

bool
readMesh(const char* filename)
{
    ifstream    is(filename);
    if (is.fail()) return false;

    char    magicNumber[256];
    is >> magicNumber;

    // # vertices, # faces, # edges
    is >> nVertices >> nFaces >> nEdges;
    cout << "# vertices = " << nVertices << endl;
    cout << "# faces = " << nFaces << endl;


    // Vertices
    vertex = new vec3[nVertices];
    for (int i = 0; i < nVertices; i++) 
        is >> vertex[i].x >> vertex[i].y >> vertex[i].z;

    // Faces
    face[0] = new int[nFaces]; 
    face[1] = new int[nFaces];
    face[2] = new int[nFaces];

    int n;
    for (int i = 0; i < nFaces; i++)
    {
        is >> n >> face[0][i] >> face[1][i] >> face[2][i];
        if (n != 3) cout << "# vertices of the " << i << "-th faces = " << n << endl;
    }

    for (int i = 0; i < nFaces; i++) {
        edges.insert({ face[0][i], face[1][i] });
        edges.insert({ face[1][i], face[2][i] });
        edges.insert({ face[2][i], face[0][i] });
    }
    cout << "# edges = " << edges.size() << endl;

    return true;
}

// Delete the vertices and faces of the mesh
void
deleteMesh()
{
    if (vertex) { delete[] vertex; vertex = NULL; }
    if (face[0]) { delete[] face[0]; face[0] = NULL; }
    if (face[1]) { delete[] face[1]; face[1] = NULL; }
    if (face[2]) { delete[] face[2]; face[2] = NULL; }

    if (!edges.empty()) { edges.clear(); }
}

// Draw the vertices of the mesh only
void
drawMesh()
{
    // Color of the points
    setDiffuseColor(vec3(0, 0, 0));

    // Place the bunny model
    glRotatef(90, 0, 0, 1);
    glRotatef(90, 1, 0, 0);

    // Draw all the vertices of the bunny model
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (const auto& edge : edges) {
        for (int k : edge) {
            glVertex3f(vertex[k].x, vertex[k].y, vertex[k].z);
        }
    }
    glEnd();
}

void
render(GLFWwindow* window)
{
    // Background color
    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    // Axes
    glDisable(GL_LIGHTING);
    drawAxes(AXIS_LENGTH, AXIS_LINE_WIDTH * dpiScaling);

    // Lighting
    setupLight();

    // Material
    setupMaterial();

    // Draw sphere using gluSphere() after setting up the material
    drawMesh();
}

void
keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            // Quit
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
        }
    }
}