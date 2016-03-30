/*****************************************************************************\
 | OpenGL Coursework 1                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where I've put commands about loading Shader files, compiling them. |
 \*****************************************************************************/

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <GL/glew.h>

using namespace std;

class Shader {
    
public:
    GLuint Program;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
        cout << "------------------------------------------------" << endl;
        cout << "======" << " Preparing Shader Program" << "======" << endl;
        Program = create_program(vertexPath, fragmentPath);
        cout << "Shader program #" << Program <<" created successfully " << endl;
        cout << "------------------------------------------------" << endl;
    }
    // Uses the current shader
    void Use() { glUseProgram(this->Program); }
private:
    GLint success;
    GLchar infoLog[512];
    // Read a shader source from a file
    // store the shader source in a std::vector<char>
    void readShaderSource(const char *fname, std::vector<char> &buffer) {
        
        cout << "reading shader from " << fname << " ..." << endl;
        
        ifstream in;
        in.open(fname, ios::binary);
        
        if(in.is_open()) {
            // Get the number of bytes stored in this file
            in.seekg(0, ios::end);
            size_t length = (size_t)in.tellg();
            
            // Go to start of the file
            in.seekg(0, ios::beg);
            
            // Read the content of the file in a buffer
            buffer.resize(length + 1);
            in.read(&buffer[0], length);
            in.close();
            // Add a valid C - string end
            buffer[length] = '\0';
        } else {
            if(!success) {
                cout << "ERROR::SHADER_SOURCE_READ_FAILED\n" << endl;
                glfwTerminate();
                exit(EXIT_FAILURE);
            }
        }
        cout << "reading successful" << endl;
    }
    GLuint makeShader(const char *fname, GLenum shaderType) {
        
        // Load a shader from an external file
        vector<char> buffer;
        readShaderSource(fname, buffer);
        const char *src = &buffer[0];
        
        // Compile the shader
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &src, NULL);
        
        cout << "compiling ... " << endl;
        glCompileShader(shader);
        
        // Check the result of the compilation
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        cout << "compilation successful " << endl;
        
        return shader;
    }
    GLuint create_program(const char *path_vert_shader, const char *path_frag_shader) {
        
        // Load and compile the vertex and fragment shaders
        GLuint vertexShader = makeShader(path_vert_shader, GL_VERTEX_SHADER);
        GLuint fragmentShader = makeShader(path_frag_shader, GL_FRAGMENT_SHADER);
        
        // Attach the above shader to a program
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        
        // Flag the shaders for deletion
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        cout << "linking shaders ..." << endl;
        
        // Link and use the program
        glLinkProgram(shaderProgram);
        
        // Print linking errors if any
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        
        cout << "linking successful" << endl;
        
        return shaderProgram;
    }
};

#endif