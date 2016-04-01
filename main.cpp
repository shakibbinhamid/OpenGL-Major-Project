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

const GLfloat ATTEN_CONST = 1.0f;
const GLfloat ATTEN_LIN   = 0.009;
const GLfloat ATTEN_QUAD  = 0.0032;

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


/////////////////////////////// HELPERS //////////////////////////////////////////////////////////////////////////////////////////

void setDirLightUniforms(Shader shader, string dirLightNr, glm::vec3 sunPos, glm::vec3 ambient, glm::vec3 diff, glm::vec3 spec){
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".direction").c_str()), sunPos.x, -sunPos.y, sunPos.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".diffuse").c_str()), diff.x, diff.y, diff.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".specular").c_str()), spec.x, spec.y, spec.z);
}

void setLampUniforms(Shader shader, string lampNr, glm::vec3 lampPos, glm::vec3 ambient, glm::vec3 diff, glm::vec3 spec){
    // light variables
    glUniform3f(glGetUniformLocation(shader.Program, (lampNr + ".position").c_str()), lampPos.x, lampPos.y, lampPos.z);
    glUniform3f(glGetUniformLocation(shader.Program, (lampNr + ".ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader.Program, (lampNr + ".diffuse").c_str()), diff.x, diff.y, diff.z);
    glUniform3f(glGetUniformLocation(shader.Program, (lampNr + ".specular").c_str()), spec.x, spec.y, spec.z);
    
    // attenuation
    glUniform1f(glGetUniformLocation(shader.Program, (lampNr + ".constant").c_str()), ATTEN_CONST);
    glUniform1f(glGetUniformLocation(shader.Program, (lampNr + ".linear").c_str()), ATTEN_LIN);
    glUniform1f(glGetUniformLocation(shader.Program, (lampNr + ".quadratic").c_str()), ATTEN_QUAD);
}

void turnOffLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setLampUniforms(shader, lampNr, lampPos, glm::vec3(), glm::vec3(), glm::vec3());
}

void turnOnLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setLampUniforms(shader, lampNr, lampPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.10f, 0.10f, 0.10f), glm::vec3(1.0f, 1.0f, 1.0f));
}

void turnOnSun(Shader shader, string lampNr, glm::vec3 sunPos){
    setDirLightUniforms(shader, lampNr, sunPos, glm::vec3(0.50f, 0.50f, 0.50f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.9f, 0.9f, 0.9f));
}

void turnOffSun(Shader shader, string lampNr, glm::vec3 sunPos){
    setDirLightUniforms(shader, lampNr, sunPos, glm::vec3(0.00f, 0.00f, 0.00f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void drawModel(Shader shader, Model Model, glm::vec3 pos = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(), GLfloat angle = 0.0f){
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, pos);
    model = glm::rotate(model, angle, rotate);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    Model.Draw(shader);
}

void drawStreetLamp(Shader shader, Model lamp, glm::vec3 pos){
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, pos);
    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    lamp.Draw(shader);
}

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
    
    Model road("models/Tree/Tree.obj");
    
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
        
        glm::vec3 sunPos = glm::vec3(100 * cos(glfwGetTime()/2), 50 * sin(glfwGetTime()/2), 0.0f); //glm::vec3(0.0f, 10.0f, 20.0f); //
        
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activated (key pressed, mouse moved)
        glfwPollEvents();
        do_movement();
        
        // Clear the color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
        glUniform1f(glGetUniformLocation(shader.Program, "Material.shininess"), 0.05);
        // Directional light
        if (sunPos.y > 0) {
            // turn on the sun
            turnOnSun(shader, "dirLight", sunPos);
            
            // turn off street lamps
            turnOffLamp(shader, "pointLights[0]", pointLampPos[0]);
            turnOffLamp(shader, "pointLights[1]", pointLampPos[1]);
            turnOffLamp(shader, "pointLights[2]", pointLampPos[2]);
            turnOffLamp(shader, "pointLights[3]", pointLampPos[3]);
        } else {
            // turn off the sun
            turnOffSun(shader, "dirLight", sunPos);
            
            // turn on the street lamps
            turnOnLamp(shader, "pointLights[0]", pointLampPos[0]);
            turnOnLamp(shader, "pointLights[1]", pointLampPos[1]);
            turnOnLamp(shader, "pointLights[2]", pointLampPos[2]);
            turnOnLamp(shader, "pointLights[3]", pointLampPos[3]);
        }
        
        // Draw the loaded model
        glm::mat4 model;
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        env.Draw(shader);
        
        model = glm::mat4();
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        road.Draw(shader);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, -0.245f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        floor.Draw(shader);
        
        drawStreetLamp(shader, lamp1, glm::vec3(pointLampPos[0].x, -0.25f, pointLampPos[0].z));
        drawStreetLamp(shader, lamp1, glm::vec3(pointLampPos[1].x, -0.25f, pointLampPos[1].z));
        drawStreetLamp(shader, lamp1, glm::vec3(pointLampPos[2].x, -0.25f, pointLampPos[2].z));
        drawStreetLamp(shader, lamp1, glm::vec3(pointLampPos[3].x, -0.25f, pointLampPos[3].z));
        
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