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
		GLfloat x, y;
		while (fin >> x >> y) {
			cameraLookOffsets.push_back(glm::vec2(x, y));
		}

		fin.close();
		return 0;
	}

	void stepTour(Camera * camera) {
		STEP++;
		if (STEP >= cameraLookOffsets.size()) {
			STEP = 0;
		}
		camera->processLook(cameraLookOffsets[STEP].x, cameraLookOffsets[STEP].y);
	}
private:
	vector<glm::vec2> cameraLookOffsets;
	int STEP;
};