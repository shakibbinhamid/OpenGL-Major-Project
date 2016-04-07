/*****************************************************************************\
 | OpenGL Coursework 1                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where I've put commands about loading Shader files, compiling them. |
 \*****************************************************************************/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include "gl_util.hpp"
#include "mesh_generator.hpp"

using namespace std;

class Skybox {
    
public:
    Skybox(vector<const GLchar*> faces, const string name = "skybox") {
        cout << "------------------------------------------------" << endl;
        cout << "======" << " Creating  Skybox " << name << " ======" << endl;
        this->faces = faces;
        this->name = name;
        init();
        cout << "Shader program #" <<" created successfully " << endl;
        cout << "------------------------------------------------" << endl;
    }
    void draw(Shader skyShader){
        render(skyShader);
    }
private:
    vector<const GLchar*> faces;
    vector<GLfloat> skyboxVertices;
    string name;
    GLuint skyboxVAO, skyboxVBO;
    GLuint skyboxTexture;
    
    void init(){
        skyboxVertices = getCubeVertices();
        skyboxTexture = loadCubemap(faces);
        
        // Setup skybox VAO
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(GLfloat), &skyboxVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glBindVertexArray(0);
    }
    
    void render(Shader skyboxShader){
        // Draw skybox as last
        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
        glm::mat4 projection = glm::perspective(camera.getZoom(), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        GLfloat brightness = sin(glfwGetTime()/2) <= 0.2 ? 0.2 : sin(glfwGetTime()/2);
        glUniform4f(glGetUniformLocation(skyboxShader.Program, "uColor"), brightness, brightness, brightness, 1.0);
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default
    }
};

#endif