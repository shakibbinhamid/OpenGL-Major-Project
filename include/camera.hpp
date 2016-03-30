/*****************************************************************************\
 | OpenGL Coursework 3                                                         |
 |                                                                             |
 | Email: sh3g12 at soton dot ac dot uk                                        |
 | version 0.0.1                                                               |
 | Copyright Shakib Bin Hamid                                                  |
 |*****************************************************************************|
 | This is where I've put commands about the camera.                           |
 | Using Euler angles                                                          |
 | This is inspired by the tutorial by  Joey de Vries at                       |
 | http://learnopengl.com/#!Getting-started/Camera                             |
 \*****************************************************************************/

#pragma once

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const GLfloat   DEFAULT_CAMERA_YAW        = -90.0f;
const GLfloat   DEFAULT_CAMERA_PITCH      =  0.0f;
const GLfloat   DEFAULT_CAMERA_SPEED      =  3.0f;
const GLfloat   DEFAULT_CAMERA_SENSITIVTY =  0.25f;
const GLfloat   DEFAULT_CAMERA_ZOOM       =  45.0f;
const glm::vec3 DEFAULT_CAMERA_POS        = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 DEFAULT_CAMERA_FRONT      = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 DEFAULT_CAMERA_UP         = glm::vec3(0.0f, 1.0f, 0.0f);

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // Constructor with vectors
    Camera(glm::vec3 position = DEFAULT_CAMERA_POS,
           glm::vec3 up = DEFAULT_CAMERA_UP,
           GLfloat yaw = DEFAULT_CAMERA_YAW,
           GLfloat pitch = DEFAULT_CAMERA_PITCH)
    :   front(DEFAULT_CAMERA_FRONT),
        lookSpeed(DEFAULT_CAMERA_SPEED),
        lookSensitivity(DEFAULT_CAMERA_SENSITIVTY),
        zoom(DEFAULT_CAMERA_ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
        render();
    }
    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
    :   front(DEFAULT_CAMERA_FRONT),
        lookSpeed(DEFAULT_CAMERA_SPEED),
        lookSensitivity(DEFAULT_CAMERA_SENSITIVTY),
        zoom(DEFAULT_CAMERA_ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
        render();
    }
    
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }
    
    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processMovement(Camera_Movement direction, GLfloat deltaTime) {
        GLfloat velocity = lookSpeed * deltaTime;
        
        if (direction == FORWARD)   position += front * velocity;
        if (direction == BACKWARD)  position -= front * velocity;
        if (direction == LEFT)      position -= right * velocity;
        if (direction == RIGHT)     position += right * velocity;
        
        position.y = 0.0f; // to restrict camera movement in xz plane
    }
    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processLook(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
        xoffset *= lookSensitivity;
        yoffset *= lookSensitivity;
        
        yaw   += xoffset;
        pitch += yoffset;
        
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (pitch > 89.0f)  pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        
        // Update Front, Right and Up Vectors using the updated Eular angles
        updateCameraVectors();
    }
    
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processZoom(GLfloat yoffset) {
        if (zoom >= 1.0f && zoom <= 45.0f) zoom -= yoffset;
        if (zoom <= 1.0f) zoom = 1.0f;
        if (zoom >= 45.0f) zoom = 45.0f;
    }
    
    glm::vec3 getPosition() {
        return position;
    }
    
    void setPosition(glm::vec3 pos) {
        position = pos;
    }
    
    GLfloat getZoom(){
        return zoom;
    }
    
    void setZoom(GLfloat zm){
        zoom = zm;
    }
    
    void render(){
        cout << "=== Camera ===" <<endl;
        cout << "Position : "   << position.x   << " , " << position.y  << " , "    << position.z   << endl;
        cout << "Front : "      << front.x      << " , " << front.y     << " , "    << front.z      << endl;
    }
    
private:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    // Eular Angles
    GLfloat yaw;
    GLfloat pitch;
    
    // Camera options
    GLfloat lookSpeed;
    GLfloat lookSensitivity;
    GLfloat zoom;
    
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        // Also re-calculate the Right and Up vector
        // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};