#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "camera.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = ",";

class Tour {
public:
	Tour() {}
	
	int loadTour(const char * tourFile) {
		ifstream fin(tourFile);
		if (!fin.good())
			return -1;
		GLfloat yaw, pitch, posx, posy, posz;
		while (fin >> yaw >> pitch >> posx >> posy >> posz) {
			cameraLookOffsets.push_back({ yaw, pitch, posx, posy, posz });
		}

		fin.close();
		return 0;
	}

	void stepTour(Camera * camera) {
		STEP++;
		if (STEP >= cameraLookOffsets.size()) {
			STEP = 0;
		}
		camera->processTourStep(cameraLookOffsets[STEP][0], cameraLookOffsets[STEP][1], cameraLookOffsets[STEP][2], cameraLookOffsets[STEP][3], cameraLookOffsets[STEP][4]);
	}
private:
	vector<vector<GLfloat>> cameraLookOffsets;
	int STEP;
};