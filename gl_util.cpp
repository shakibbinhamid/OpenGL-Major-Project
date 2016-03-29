/*****************************************************************************\
 | OpenGL Coursework 1                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where I've put commands about the glew and glfw init.               |
 | Also, the camera commands, keyboard and mouse movements are declared here.  |
 \*****************************************************************************/

#include "gl_util.hpp"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

using namespace std;

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)        keys[key] = true;
        else if(action == GLFW_RELEASE) keys[key] = false;
    }
}

////////////////////// GLEW AND GLFW SCAFFOLDING //////////////////////////////////////////////////
bool start_gl () {
    
    cout << "------------------------------------------------" << endl;
    cout << "starting GLFW " << glfwGetVersionString () << endl;
    
    // Init GLFW
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return false;
    }
    
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    window = glfwCreateWindow (WIDTH, HEIGHT, "Coursework 3", nullptr, nullptr);
    
    if (!window) {
        fprintf (stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent (window);
    glfwWindowHint (GLFW_SAMPLES, 4);
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    
    /* start GLEW extension handler */
    glewExperimental = GL_TRUE;
    
    glewInit ();
    
    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);
    
    // Enable testing for depth so that stuff in the back are not drawn
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
    
    cout << "------------------------------------------------" << endl;
    cout << "========= OpenGL info ========="   << endl;
    cout << "Renderer: "                        << glGetString (GL_RENDERER) << endl;
    cout << "OpenGL version supported "         << glGetString (GL_VERSION)  << endl;
    cout << "------------------------------------------------" << endl;
    
    return true;
}

void do_movement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.processMovement(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.processMovement(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.processMovement(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.processMovement(RIGHT, deltaTime);
    if(keys[GLFW_KEY_UP])
        camera.processLook(0, 2);
    if(keys[GLFW_KEY_DOWN])
        camera.processLook(0, -2);
    if(keys[GLFW_KEY_LEFT])
        camera.processLook(-2, 0);
    if(keys[GLFW_KEY_RIGHT])
        camera.processLook(2, 0);
}
