/*****************************************************************************\
 | OpenGL Coursework 3                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where I've put commands about the glew and glfw init.               |
 | Also, the camera commands, keyboard and mouse movements are declared here.  |
 \*****************************************************************************/
#ifndef _GL_UTIL_H_
#define _GL_UTIL_H_

#include <GL/glew.h> /* include GLEW and new version of GL on Windows */
#include <GLFW/glfw3.h> /* GLFW helper library */
#include "camera.hpp"
#include "tour.hpp"

extern GLuint WIDTH;
extern GLuint HEIGHT;
extern GLFWwindow* window;

extern Camera camera;
extern bool keys[];

extern GLfloat deltaTime;
extern GLfloat lastFrame;

bool start_gl ();

void do_movement();

GLint Texture2DFromFile(string filename);

GLint getMaxTextureSupported();

GLuint loadCubemap(vector<const GLchar*> faces);

#endif
