/*****************************************************************************\
 | OpenGL Coursework 3                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where everything comes together.                                    |
 | I've written all the sphere calculation and drawing commands here           |
 \*****************************************************************************/
#define _USE_MATH_DEFINES
#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)

#include <cmath>
#include <iostream>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "stb_image.h"
#include "shader.hpp"
#include "camera.hpp"
#include "gl_util.hpp"
#include "model.hpp"
#include "mesh_generator.hpp"

///////////////////////////////// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////////

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow* window = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f)); //-4.02939f, 10.0f, -34.2374f
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// for spheres
GLint stacks = 100;
GLint slices = 100;
GLfloat radius = 1.0f;

// light source position
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);


/////////////////////////////// PROTOTYPES //////////////////////////////////////////////////////////////////////////////////////////

// generates a sphere and populates vertices, indices. q2verts are only used in question 2
Mesh generateSphere (GLint Stacks, GLint Slices, GLfloat r);

////////////////////////////// MAIN FUNCTION /////////////////////////////////////////////////////////////////////////////////////////

// The MAIN function, from here we start the application and run the game loop
int main() {
    
    // start glfw and glew with default settings
    assert(start_gl());
    
    // Build and compile our shader program
    Shader shader("shaders/shader.vs",
                        "shaders/shader.frag");
    
    Shader sunShader("shaders/sun.vs",
                     "shaders/sun.frag");
    //GLchar * path = "/Users/shakib-binhamid/Downloads/nanosuit";
    // Load models
    Model env("models/environment/Street environment_V01.obj");
    
    Model lamp1("models/streetlamp/streetlamp.obj");
    
    Mesh sun = generateUVSphere(50, 50, 5.00);
    sun.addTextureFromFile("images/sunmap.jpg",
                           "material.texture_diffuse");
    
    Mesh floor = generateRectangularFloor(20, 20, 0);
    floor.addTextureFromFile("images/concrete.jpg",
                           "material.texture_diffuse");
    floor.addTextureFromFile("images/concrete_spec.jpeg",
                             "material.texture_specular");
    
    /////////////////  The positions for the spheres in q4  ////////////////////////////////////////////

    // Point light positions
    glm::vec3 pointLampPos[] = {
        glm::vec3(2.9f, 0.25f, -2.8f),
        glm::vec3(-2.9f, 0.25f, -3.4f),
        glm::vec3(-2.7f, 0.25f, 2.8f),
        glm::vec3(2.5f, 0.25f, 2.8f)
    };
    
    /////////////////  Uniform variables for MVP in VS  /////////////////////////////////////////////////
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        
        glm::vec3 sunPos = glm::vec3(100 * cos(glfwGetTime()/2), 50 * sin(glfwGetTime()/2), 0.0f);
        
        if (sunPos.y < -30) sunPos.y = -200;
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activated (key pressed, mouse moved)
        glfwPollEvents();
        do_movement();
        
        // Clear the color buffer
        glClearColor(sin(glfwGetTime()/2), sin(glfwGetTime()/2), 0.0f, 0.0f);
        //glClearColor(sin(glfwGetTime()/2), sin(glfwGetTime()/2)/2, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.Use();   // <-- Don't forget this one!
        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.getZoom(), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        // Directional light
        if (sunPos.y > 0) {
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.direction"), sunPos.x, -sunPos.y, sunPos.z);
            glUniform1f(glGetUniformLocation(shader.Program, "Material.shininess"), 0.05);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.diffuse"), 0.9f, 0.9f, 0.9f);
            glUniform3f(glGetUniformLocation(shader.Program, "dirLight.specular"), 0.9f, 0.9f, 0.9f);
            
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLampPos[0].x, pointLampPos[0].y, pointLampPos[0].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 0.00f, 0.00f, 0.00f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.0032);
            // Point light 2
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLampPos[1].x, pointLampPos[1].y, pointLampPos[1].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 0.00f, 0.00f, 0.00f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.0032);
            // Point light 3
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].position"), pointLampPos[2].x, pointLampPos[2].y, pointLampPos[2].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].diffuse"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].specular"), 0.00f, 0.00f, 0.00f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].quadratic"), 0.0032);
            // Point light 4
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].position"), pointLampPos[3].x, pointLampPos[3].y, pointLampPos[3].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].diffuse"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].specular"), 0.00f, 0.00f, 0.00f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].quadratic"), 0.0032);
        } else {
//        // Point light 1
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLampPos[0].x, pointLampPos[0].y, pointLampPos[0].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.10f, 0.10f, 0.10f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.0032);
            // Point light 2
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLampPos[1].x, pointLampPos[1].y, pointLampPos[1].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 0.10f, 0.10f, 0.10f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.0032);
            // Point light 3
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].position"), pointLampPos[2].x, pointLampPos[2].y, pointLampPos[2].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].diffuse"), 0.10f, 0.10f, 0.10f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].quadratic"), 0.0032);
            // Point light 4
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].position"), pointLampPos[3].x, pointLampPos[3].y, pointLampPos[3].z);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].ambient"), 0.00f, 0.00f, 0.00f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].diffuse"), 0.10f, 0.10f, 0.10f);
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].quadratic"), 0.0032);
        }
        
        // Draw the loaded model
        glm::mat4 model;
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        env.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, -0.245f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        floor.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(pointLampPos[0].x, -0.25f, pointLampPos[0].z));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp1.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(pointLampPos[1].x, -0.25f, pointLampPos[1].z));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp1.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(pointLampPos[2].x, -0.25f, pointLampPos[2].z));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp1.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(pointLampPos[3].x, -0.25f, pointLampPos[3].z));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamp1.Draw(shader);
        
        sunShader.Use();
        // Draw the loaded model
        model = glm::mat4();
        model = glm::translate(model, sunPos); // Translate it down a bit so it's at the center of the scene
        glUniformMatrix4fv(glGetUniformLocation(sunShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(sunShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(sunShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sun.Draw(sunShader);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}