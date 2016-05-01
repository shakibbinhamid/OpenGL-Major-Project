#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "camera.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Tour {
public:
	Tour(const char * tourFile, const char * initFile) {
		loadTour(tourFile, initFile);
		cout << "TOUR LOADED : STEPS = " << cameraParamOffsets.size() << endl;
	}
	
	int loadTour(const char * tourFile, const char * initFile) {

		// load tour offsets
		ifstream tf(tourFile);
		if (!tf.good())
			return -1;
		GLfloat f, b, l, r, yp, yn, xn, xp;
		while (tf >> f >> b >> l >> r >> yp >> yn >> xn >> xp) {
			cameraParamOffsets.push_back({ f, b, l, r, yp, yn, xn, xp });
		}

		tf.close();

		// load init conditions
		ifstream inf(initFile);
		if (!inf.good())
			return -1;
		GLfloat sp_x, sp_y, sp_z, sd_y, sd_p;
		while (inf >> sp_x >> sp_y >> sp_z >> sd_y >> sd_p) {
			start_position = glm::vec3(sp_x, sp_y, sp_z);
			start_direction = glm::vec2(sd_y, sd_p);
		}

		inf.close();
		return 0;
	}

	GLboolean tourLoaded() {
		return cameraParamOffsets.size() > 0;
	}

	void restartTour(Camera * camera) {
		stopTour(); // reset steps if at the end
		camera->setUpTour(start_direction.x, start_direction.y, start_position.x, start_position.y, start_position.z);

		cout << "Tour Started" << endl;
	}

	void stopTour() {
		STEP = 0;
		cout << "Tour stopped" << endl;
	}

	void stepTour(Camera * camera, GLfloat deltaTime) {
		if (STEP >= cameraParamOffsets.size() || STEP == 0) restartTour(camera);

		camera->stepTour(cameraParamOffsets[STEP][0], cameraParamOffsets[STEP][1], cameraParamOffsets[STEP][2], cameraParamOffsets[STEP][3],
						 cameraParamOffsets[STEP][4], cameraParamOffsets[STEP][5], cameraParamOffsets[STEP][6], cameraParamOffsets[STEP][7],
						 deltaTime);

		STEP++;
	}
private:
	vector<vector<GLfloat>> cameraParamOffsets;
	glm::vec3 start_position, last_position;
	glm::vec2 start_direction, last_direction;
	int STEP;
};