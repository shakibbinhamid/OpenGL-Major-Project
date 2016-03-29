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
#include "shader.h"
#include "camera.h"
#include "gl_util.hpp"
#include "model.h"

///////////////////////////////// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////////

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow* window = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
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
    Shader shader("shaders/shader.vs", "shaders/shader.frag");
    
    Shader lampShader("shaders/shader.vs", "shaders/lamp.frag");
    //GLchar * path = "/Users/shakib-binhamid/Downloads/nanosuit";
    // Load models
    Model ourModel("models/nanosuit/nanosuit.obj");
    
    /////////////////  The positions for the spheres in q4  ////////////////////////////////////////////

    // Point light positions
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, -5.0f)
    };
    
    /////////////////  Uniform variables for MVP in VS  /////////////////////////////////////////////////
    
    GLint modelLoc = glGetUniformLocation(shader.Program, "model");
    GLint viewLoc = glGetUniformLocation(shader.Program, "view");
    GLint projLoc = glGetUniformLocation(shader.Program, "projection");
    
    // uniforms for lighting
    GLint lightPosLoc = glGetUniformLocation(shader.Program, "light.position");
    GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activated (key pressed, mouse moved)
        glfwPollEvents();
        do_movement();
        
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.Use();   // <-- Don't forget this one!
        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.getZoom(), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        // Set the lighting uniforms
        glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        // Point light 1
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.009);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.0032);
        // Point light 2
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.009);
        glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.0032);
        
        // Draw the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        ourModel.Draw(shader);
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

//////////////////////////////////////// DRAWING COMMAND //////////////////////////////////////////////////////////////////////////////////

void drawSphere(Shader * shader, GLuint * sphere_VAO, std::vector<GLint> * sphere_idx,
                Shader * lampShader,
                GLint * lightPosLoc, GLint * viewPosLoc,
                GLuint * diff_texture, GLuint * spec_texture,
                GLuint count, glm::vec3 * locations, GLint * modelLoc, GLint * viewLoc, GLint * projLoc) {
    
    // Activate shader
    shader->Use();
    
    // camera position
    glUniform3f(*viewPosLoc, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z); // camera position for spec light
    
    // light source position
    glUniform3f(*lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    
    // sphere material
    GLint matDiffLoc = glGetUniformLocation(shader->Program, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(shader->Program, "material.specular");
    GLint matShineLoc    = glGetUniformLocation(shader->Program, "material.shininess");
    
    glUniform1i(matDiffLoc, 0);
    glUniform1i(matSpecularLoc, 1);
    glUniform1f(matShineLoc,    32.0f);
    
    glUniform1f(glGetUniformLocation(shader->Program, "light.constant"),  1.0f);
    glUniform1f(glGetUniformLocation(shader->Program, "light.linear"),    0.09);
    glUniform1f(glGetUniformLocation(shader->Program, "light.quadratic"), 0.032);
    
    // light source
    GLint lightAmbientLoc  = glGetUniformLocation(shader->Program, "light.ambient");
    GLint lightDiffuseLoc  = glGetUniformLocation(shader->Program, "light.diffuse");
    GLint lightSpecularLoc = glGetUniformLocation(shader->Program, "light.specular");
    
    glUniform3f(lightAmbientLoc,  0.2f, 0.2f, 0.2f);
    glUniform3f(lightDiffuseLoc,  0.5f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene
    glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

    // Pass the view and projection matrices to the shader
    glm::mat4 view = camera.GetViewMatrix(); // Camera/View transformation
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f); // Projection
    glUniformMatrix4fv(*viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(*projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // place the sphere in the right place
    glBindVertexArray(*sphere_VAO);
    
    // Bind Textures using texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *diff_texture);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *spec_texture); // TODO: change this to specMap
    
    // larger sphere
    glm::mat4 model; // model
    
    for (int i = 0; i < count; i++) {
        model = glm::mat4();
        model = glm::translate(model, locations[i]);
        model = glm::rotate(model, (GLfloat)glfwGetTime() * 2, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(*modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(shader->Program, "ourTexture1"), 0);
        glDrawElements(GL_TRIANGLES, (GLint)sphere_idx->size(), GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0); // done drawing sphere, unload VAO
}

///////////////////////////////// HELPER FUNCTIONS //////////////////////////////////////////////////////////////////////////

/*
 Generates a sphere and populates the vertices, indices based on how many 'stacks' and 'slices' are needed.
 It is a UV sphere.
 vertices contain position, normal, texcord
 q2 verts just contain position, normal
 */
Mesh generateSphere (const GLint Stacks, const GLint Slices, const GLfloat r){
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    for (int i = 0; i <= Stacks; ++i){
        float V   = i / (float) Stacks;
        float phi = V * glm::pi <float> ();
        // Loop Through Slices
        for (int j = 0; j <= Slices; ++j){
            float U = j / (float) Slices;
            float theta = U * (glm::pi <float> () * 2);
            
            // Calc The Vertex Positions
            float x = r * cosf (theta) * sinf (phi);
            float y = r * cosf (phi);
            float z = r * sinf (theta) * sinf (phi);
            
            // vertices for sphere
            Vertex v;
            v.position  = glm::vec3(x, y, z);
            v.normal    = glm::vec3(v.position + glm::normalize(v.position) * 0.05f);
            v.texCoords = glm::vec2 (U, V);
            
            vertices.push_back(v);
        }
    }
    
    for (int i = 0; i < Slices * Stacks + Slices; ++i){
        indices.push_back (i);
        indices.push_back (i + Slices + 1);
        indices.push_back (i + Slices);
        
        indices.push_back (i + Slices + 1);
        indices.push_back (i);
        indices.push_back (i + 1);
    }
    
    return Mesh(vertices, indices, textures);
}