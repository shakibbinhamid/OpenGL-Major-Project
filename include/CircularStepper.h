#pragma once

#include <vector>
#include "GL/glew.h"
#include "glm/glm.hpp"

#define PI 3.14159265

using namespace std;

class CircularStepper {
public:
	CircularStepper(GLfloat min = 0.0f, GLfloat max = 2 * PI, GLuint steps = 1000) {
		this->min = min;
		this->max = max;

		GLfloat inc = (max - min) / steps;
		for (GLuint i = 0; i < steps; i++) {
			min += inc;
			param.push_back(min);
		}
	}
	GLfloat step() {
		currentStep++;
		if (currentStep >= param.size()) currentStep = 0;
		return param[currentStep];
	}
	GLfloat getCurrentStep(){
		return param[currentStep];
	}
private:
	vector<GLfloat> param;
	GLfloat min;
	GLfloat max;
	GLuint currentStep = 0;
};