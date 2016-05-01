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
#include "stb_image.h"

GLint max_texture_units;
Tour t("tourRoute.txt", "tourInit.txt");
GLboolean record = false, tourMode = false;

const int toggleKeys[] = { GLFW_KEY_T, GLFW_KEY_V };

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	bool toggleKey = std::find(std::begin(toggleKeys), std::end(toggleKeys), key) != std::end(toggleKeys);
	if (toggleKey && action == GLFW_PRESS) keys[key] = !keys[key];
    else if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)						keys[key] = true;
        else if( !toggleKey && action == GLFW_RELEASE)  keys[key] = false;
    }

	if (key == GLFW_KEY_SPACE) keys[GLFW_KEY_T] = false;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) t.restartTour(&camera);
	if (key == GLFW_KEY_E) {
		keys[GLFW_KEY_T] = false;
		t.stopTour();
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
    
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
    
    cout << "------------------------------------------------" << endl;
    cout << "========= OpenGL info ========="    << endl;
    cout << "Renderer: "                         << glGetString (GL_RENDERER) << endl;
    cout << "OpenGL version supported "          << glGetString (GL_VERSION)  << endl;
    cout << "Max # of texture units supported : "<< max_texture_units         << endl;
    cout << "------------------------------------------------" << endl;
    
    return true;
}

GLint getMaxTextureSupported(){
    return max_texture_units;
}

GLboolean isTouring() {
	return tourMode;
}

void do_movement() {
	tourMode = keys[GLFW_KEY_T];
	record = keys[GLFW_KEY_V];
	if (tourMode) {
		record = keys[GLFW_KEY_V] = false;
		camera.setRecording(false);
	}
	if (record) {
		tourMode = keys[GLFW_KEY_T] = false;
		if (!camera.isRecording()) {
			camera.setRecording(true);
			camera.recordTourInit();
		}
	}
	if (tourMode) {
		t.stepTour(&camera, deltaTime);
	}
	else {
		// Camera controls
		if (keys[GLFW_KEY_W])
			camera.processMovement(FORWARD, deltaTime);
		if (keys[GLFW_KEY_S])
			camera.processMovement(BACKWARD, deltaTime);
		if (keys[GLFW_KEY_A])
			camera.processMovement(LEFT, deltaTime);
		if (keys[GLFW_KEY_D])
			camera.processMovement(RIGHT, deltaTime);
		if (keys[GLFW_KEY_UP])
			camera.processLook(0, 1);
		if (keys[GLFW_KEY_DOWN])
			camera.processLook(0, -1);
		if (keys[GLFW_KEY_LEFT])
			camera.processLook(-1, 0);
		if (keys[GLFW_KEY_RIGHT])
			camera.processLook(1, 0);
		if (camera.isRecording())
			camera.recordTourStep(keys[GLFW_KEY_W] ? 1 : 0,
				keys[GLFW_KEY_S] ? 1 : 0,
				keys[GLFW_KEY_A] ? 1 : 0,
				keys[GLFW_KEY_D] ? 1 : 0,
				keys[GLFW_KEY_UP] ? 1 : 0,
				keys[GLFW_KEY_DOWN] ? 1 : 0,
				keys[GLFW_KEY_LEFT] ? 1 : 0,
				keys[GLFW_KEY_RIGHT] ? 1 : 0);
	}
}

/*
 Will connect rgba texture to GL_TEXTURE_2D, GL_REPEAT (both s, t), GL_LINEAR filtering
 */
GLint Texture2DFromFile(string filename) {
    
    // prepare right path for texture data
    
    //Generate texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width,height, comp;
    
    cout << "loading texture from " << filename.c_str() << endl;
    
    // Load, create texture and generate mipmaps
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb);
    if(image == nullptr)
        throw(std::string("Failed to load texture from " + filename));
    cout << "texture #"<< textureID << " loaded successfully " << filename.c_str() << endl;
    
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // delete image data
    stbi_image_free(image);
    return textureID;
}

GLuint loadCubemap(vector<const GLchar*> faces){
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width,height;
    unsigned char* image = nullptr;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++) {
        image = stbi_load(faces[i], &width, &height, 0, STBI_rgb);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        if(image == nullptr)
            throw(std::string("Failed to load texture from ") + std::string(faces[i]));
        cout << "texture #"<< textureID << " loaded successfully " << faces[i] << endl;
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    stbi_image_free(image);
    
    return textureID;
}
