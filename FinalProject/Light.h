#pragma once

#include "GL/glut.h"

class Light {
public:
	Light(float x, float y, float z, int lightID);
	const int getLightID() const;
	void setPos(float x, float y, float z);
	const float* getPos() const;
	void draw() const;
	
private:
	int lightID;
	float position[3];
	float ambient[4];
	float diffuse[4];
	float specular[4];
};