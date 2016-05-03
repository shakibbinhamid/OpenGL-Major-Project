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
#include <bullet/btBulletDynamicsCommon.h>
#include "stb_image.h"
#include "shader.hpp"
#include "camera.hpp"
#include "gl_util.hpp"
#include "model.hpp"
#include "mesh_generator.hpp"
#include "skybox.hpp"
#include "PhysicsWorld.hpp"
#include "CircularStepper.h"

const GLfloat ATTEN_CONST = 1.0f;
const GLfloat ATTEN_LIN   = 0.009f;
const GLfloat ATTEN_QUAD  = 0.0032f;
const GLfloat LAMP_MODEL_X_OFFSET = 0.1f;
const GLuint HOUSE_NUMBER = 11;

//------------------------------------------------- globals -------------------------------------------------------------------------------------------------------------------------------------------/

// Point light positions
glm::vec3 pointLampPos[] = {
    glm::vec3(2.9f, 0.50f, -2.8f),
    glm::vec3(-2.9f, 0.50f, -3.4f),
    glm::vec3(-2.7f, 0.50f, 2.8f),
    glm::vec3(2.5f, 0.50f, 2.8f),
    glm::vec3(-3.0f, 0.5f, -7.8f),
    glm::vec3(-7.2f, 0.5f, -2.5f),
    glm::vec3(8.0f, 0.5f, 1.8f),
};

glm::vec3 treePos[] = {
	glm::vec3(4.5f, 0.0f, 1.8f),
	glm::vec3(7.0f, 0.0f, 1.8f),
	glm::vec3(-6.0f, 0.0f, 1.8f),
	glm::vec3(-2.5f, 0.0f, 5.0f),
	glm::vec3(-3.5f, 0.0f, 9.0f)
};

glm::vec3 spotLampPos[] = {
    glm::vec3(2.5f, 0.95f, -7.5f)
};

GLfloat houseSizes[HOUSE_NUMBER][3] = {
	{ 2, 3.5, 1.715 },
	{ 4.4, 1.715, 3.5 },
	{ 1.4, 2.015, 3.0 },
	{ 5.0, 2.015, 2.5 },

	{ 2.0, 1.015, 3.3 },
	{ 2.0, 1.015, 3.3 },
	{ 2.0, 1.015, 3.3 },
	{ 2.7, 1.015, 2.1 },
	{ 2.1, 1.015, 2.7 },
	{ 0.9, 0.5515, 1.4 },

	{ 2.0, 1.115, 4.6 }
};

glm::vec3 housePos[HOUSE_NUMBER] = {
	glm::vec3(3.1, 0.8575, 4.0),
	glm::vec3(7.3, 0.8575, 3.2),
	glm::vec3(2.8, 1.0075, 7.85),
	glm::vec3(6.9, 1.0075, 7.85),

	glm::vec3(3.2, 0.5075, -7.5),
	glm::vec3(6.0, 0.5075, -7.5),
	glm::vec3(8.4, 0.5075, -7.5),
	glm::vec3(3.55, 0.5075, -4.0),
	glm::vec3(6.15, 0.5075, 2.7),
	glm::vec3(8.2, 0.27575, -3.7),

	glm::vec3(-4.0, 0.5575, -5.3)
};

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow* window = nullptr;

// Camera
Camera camera(glm::vec3(0.0f, 0.5f, 0.0f)); //-4.02939f, 10.0f, -34.2374f
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// for spheres
GLint stacks = 100;
GLint slices = 100;
GLfloat radius = 1.0f;

CircularStepper * droneStepper = new CircularStepper();

//------------------------------------------------- bullet methods --------------------------------------------------------------------------------------------------------------------------------------------/

//----------------------------------- uniform setter helpers ---------------------------------------------------------------/

void setDirLightUniforms(Shader shader, string dirLightNr, glm::vec3 sunPos, glm::vec3 ambient, glm::vec3 diff, glm::vec3 spec){
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".direction").c_str()), sunPos.x, -sunPos.y, sunPos.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".ambient").c_str()), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".diffuse").c_str()), diff.x, diff.y, diff.z);
    glUniform3f(glGetUniformLocation(shader.Program, (dirLightNr + ".specular").c_str()), spec.x, spec.y, spec.z);
}

void setPointLampUniforms(Shader shader, string lampNr, glm::vec3 lampPos, glm::vec3 ambient, glm::vec3 diff, glm::vec3 spec){
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

void setSpotLampUniforms(Shader shader, string lampNr, glm::vec3 lampPos, glm::vec3 dir, glm::vec3 ambient, glm::vec3 diff, glm::vec3 spec){
    glUniform3f(glGetUniformLocation(shader.Program, "spotLight.position"), lampPos.x, lampPos.y, lampPos.z);
    glUniform3f(glGetUniformLocation(shader.Program, "spotLight.direction"), dir.x, dir.y, dir.z);
    glUniform3f(glGetUniformLocation(shader.Program, "spotLight.ambient"), ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(shader.Program, "spotLight.diffuse"), diff.x, diff.y, diff.z);
    glUniform3f(glGetUniformLocation(shader.Program, "spotLight.specular"), spec.x, spec.y, spec.z);

    glUniform1f(glGetUniformLocation(shader.Program, "spotLight.constant"), ATTEN_CONST);
    glUniform1f(glGetUniformLocation(shader.Program, "spotLight.linear"), ATTEN_LIN);
    glUniform1f(glGetUniformLocation(shader.Program, "spotLight.quadratic"), ATTEN_QUAD);

    glUniform1f(glGetUniformLocation(shader.Program, "spotLight.cutOff"), glm::cos(glm::radians(40.5f)));
    glUniform1f(glGetUniformLocation(shader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(50.0f)));
}

void turnOffPointLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setPointLampUniforms(shader, lampNr, lampPos, glm::vec3(), glm::vec3(), glm::vec3());
}

void turnOnPointLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setPointLampUniforms(shader, lampNr, lampPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.10f, 0.10f, 0.10f), glm::vec3(1.0f, 1.0f, 1.0f));
}

void turnOffSpotLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setSpotLampUniforms(shader, lampNr, lampPos, glm::vec3(), glm::vec3(), glm::vec3(), glm::vec3());
}

void turnOnSpotLamp(Shader shader, string lampNr, glm::vec3 lampPos){
    setSpotLampUniforms(shader, lampNr, lampPos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.00f, 1.00f, 1.00f), glm::vec3(1.0f, 1.0f, 1.0f));
}

void turnOnSun(Shader shader, string lampNr, glm::vec3 sunPos){
    setDirLightUniforms(shader, lampNr, sunPos, glm::vec3(0.50f, 0.50f, 0.50f), glm::vec3(0.9f, 0.9f, 0.9f), glm::vec3(0.9f, 0.9f, 0.9f));
}

void turnOffSun(Shader shader, string lampNr, glm::vec3 sunPos){
    setDirLightUniforms(shader, lampNr, sunPos, glm::vec3(0.00f, 0.00f, 0.00f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

glm::mat4 getModelMat(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, GLfloat angle){
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, pos);
    model = glm::rotate(model, angle, rotate);
    model = glm::scale(model, scale);
    return model;
}

//----------------------------------- redering methods -------------------------------------------------------------------------------------------------------------------------------------------------------/

void drawModel(Shader shader, Model Model, glm::vec3 pos = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat angle = 0.0f){
    shader.Use();
    glm::mat4 model = getModelMat(pos, scale, rotate, angle);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    Model.Draw(shader);
}

void drawMesh(Shader shader, Mesh Mesh, glm::vec3 pos = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat angle = 0.0f){
    shader.Use();
    glm::mat4 model = getModelMat(pos, scale, rotate, angle);
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    Mesh.Draw(shader);
}

void drawStreetLamp(Shader shader, Model lamp, glm::vec3 pos){
    drawModel(shader, lamp, glm::vec3(pos.x + LAMP_MODEL_X_OFFSET, pos.y, pos.z), glm::vec3(0.05f));
}

void drawRobot(Shader shader, Model robot, glm::vec3 pos){
    drawModel(shader, robot, pos, glm::vec3(0.0025f));
}

void drawSrvDrone(Shader shader, Model drone, glm::vec3 pos, GLfloat angle) {
	drawModel(shader, drone, pos, glm::vec3(0.0025f), glm::vec3(0.0f, 1.0f, 0.0f), angle);
}

void drawAtkDrone(Shader shader, Model drone, glm::vec3 pos) {
	drawModel(shader, drone, pos, glm::vec3(0.5f));
}

void drawDebris(Shader shader, Model bus, glm::vec3 pos){
    drawModel(shader, bus, pos, glm::vec3(0.05f));
}

void drawEnvironment(Shader shader, Model env, glm::vec3 pos){
    drawModel(shader, env, pos, glm::vec3(0.2f));
}

void drawSun(Shader sunShader, Mesh sun, glm::mat4 projection, glm::mat4 view, glm::vec3 sunPos){
    // the sun
    sunShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(sunShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(sunShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    drawMesh(sunShader, sun, sunPos);
}

//----------------------------------- complete scene render method ---------------------------------//
void sceneRender(Shader shader, Shader sunShader,
                 glm::mat4 projection, glm::mat4 view,
                 Model env, Model lamp, Model tree, Model debris,
				 Model robot, Model atkDrone, Model srvDrone,
                 Mesh floor, Mesh sun){
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Set the lighting uniforms
    glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    glUniform1f(glGetUniformLocation(shader.Program, "Material.shininess"), 0.05f);
    // Directional light
    glm::vec3 sunPos = glm::vec3(100 * cos(glfwGetTime()/2), 50 * sin(glfwGetTime()/2), 0.0f);
    if (sunPos.y > 0) {
        // turn on the sun
        turnOnSun(shader, "dirLight", sunPos);
		// turn off the spotlight
        turnOffSpotLamp(shader, "spotlight", spotLampPos[0]);

        // turn off street lamps
		for (int i = 0; i < 7; i++) {
			turnOffPointLamp(shader, string("pointLights[") + to_string(i) + string("]"), pointLampPos[i]);
		}
    } else {
        // turn off the sun
        turnOffSun(shader, "dirLight", sunPos);
		// turn on the spotlight
        turnOnSpotLamp(shader, "spotlight", spotLampPos[0]);

        // turn on the street lamps
		for (int i = 0; i < 7; i++) {
			turnOnPointLamp(shader, string("pointLights[") + to_string(i) + string("]"), pointLampPos[i]);
		}
    }
    // environment
    drawEnvironment(shader, env, glm::vec3(0.0f, 0.0f, 0.0f));
    // floor
    drawMesh(shader, floor, glm::vec3(0.0f, 0.015f, 0.0f));
    // street lamps
	for (int i = 0; i < 7; i++) {
		drawStreetLamp(shader, lamp, glm::vec3(pointLampPos[i].x, 0.0f, pointLampPos[i].z));
	}
    // debris
    drawDebris(shader, debris, glm::vec3(1.0f, 0.0f, -8.5f));
    // trees
	for (int i = 0; i < 5; i++) {
		drawModel(shader, tree, treePos[i], glm::vec3(0.2f));
	}

	droneStepper->step();

	GLfloat srv_x = glm::sin(droneStepper->getCurrentStep()) + 2 * glm::sin(2 * droneStepper->getCurrentStep());
	GLfloat srv_y = glm::cos(droneStepper->getCurrentStep()) - 2 * glm::cos(2 * droneStepper->getCurrentStep());
	GLfloat srv_z = -glm::sin(3 * droneStepper->getCurrentStep());

    drawRobot(shader, robot, glm::vec3(0.0f, 0.0f, srv_y));
	drawSrvDrone(shader, srvDrone, glm::vec3(srv_x, srv_z + 2, srv_y), srv_x);
	drawAtkDrone(shader, atkDrone, glm::vec3(5 * sin(glfwGetTime()), 2, 5 * cos(glfwGetTime())));

    // the sun
    drawSun(sunShader, sun, projection, view, sunPos);
}

//---------------------------------------------------- main method --------------------------------------------------------------------------------------------------------------------------------------------/

// The MAIN function, from here we start the application and run the game loop
int main() {

    //----------------------------------- initiate opengl ---------------------------------------------------------------/

    // start glfw and glew with default settings
    bool initGL = start_gl();
    assert(initGL);

    //----------------------------------- Shaders ---------------------------------------------------------------/
    // Build and compile our shader program
    Shader shader("shaders/shader.vs",
                        "shaders/shader.frag");

    Shader sunShader("shaders/sun.vs",
                     "shaders/sun.frag");

    Shader skyboxShader("shaders/skybox.vs",
                        "shaders/skybox.frag");
    //----------------------------------- Model and Mesh ---------------------------------------------------------------/
    Model env("models/environment/Street environment_V01.obj", "Houses");
    Model straightLamp("models/streetlamp/streetlamp.obj", "Straight Lamps");
    Model tree("models/Tree/Tree.obj", "Tree");
    Model robot("models/BB8/bb8.obj", "BB 8");
	Model atkDrone("models/drone/Drone.obj", "Attack Drone");
	Model srvDrone("models/drone-mq/MQ-27.obj", "Survailance Drone");
    Model debris("models/debris/Item01.obj", "Debris");
    Mesh sun = generateUVSphere(50, 50, 2.00, "sun");
    sun.addTextureFromFile("images/sunmap.jpg",
                           "material.texture_diffuse");
	Mesh floor = generateRectangularFloor(20.0, 20.0, 0, "roads");
	floor.addTextureFromFile("images/concrete2.jpg",
                           "material.texture_diffuse");
    floor.addTextureFromFile("images/concrete_spec.jpeg",
                             "material.texture_specular");

    // ------------------------------------ Skyboxes ------------------------------------------------------------------//
    vector<const GLchar*> faces;
    faces.push_back("models/mp_pr/pr_ft.tga");
    faces.push_back("models/mp_pr/pr_bk.tga");
    faces.push_back("models/mp_pr/pr_up.tga");
    faces.push_back("models/mp_pr/pr_dn.tga");
    faces.push_back("models/mp_pr/pr_rt.tga");
    faces.push_back("models/mp_pr/pr_lf.tga");

    Skybox cityscape(faces, "Cityscape");

	// ------------------------------------ Physics ------------------------------------------------------------------//

	Physics physics;
	physics.addModel(env, glm::vec3(0.0f, -1.0f, 0.0f));

	//physics.addSphere("sphere 2", 1, 1, glm::vec3(0, 20, 0));
	//physics.addSphere("sphere 3", 1, 1, glm::vec3(0, 10, 0));
 //   physics.addSide("floor", glm::vec3(0, 1, 0), glm::vec3(0, 0.015, 0));
	////physics.addBox("House 1", 2, 3.5, 1.715, glm::vec3(3.1, 1.715/2, 4.0));

	//for (int i = 0; i < HOUSE_NUMBER; i++) {
	//	physics.addBox("House " + to_string(i), houseSizes[i][0], houseSizes[i][1], houseSizes[i][2], housePos[i]);
	//}

	printHelp();
	
    //----------------------------------- Game loop ---------------------------------------------------------------/

    // Main loop
    while (!glfwWindowShouldClose(window)) {

        GLfloat currentFrame = (GLfloat)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activated (key pressed, mouse moved)
        glfwPollEvents();
        do_movement();

        // Clear the color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        //glClearColor(sin(glfwGetTime()/2), sin(glfwGetTime()/2)/2, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.getZoom(), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

		physics.simulate();

        sceneRender(shader, sunShader,
                    projection, view,
                    env, straightLamp, tree, debris,
					robot, atkDrone, srvDrone,
                    floor, sun);

        cityscape.draw(skyboxShader);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    // Terminate GLFW
    glfwDestroyWindow(window);
	//physics.shutdown();
    glfwTerminate();
    return EXIT_SUCCESS;
}
