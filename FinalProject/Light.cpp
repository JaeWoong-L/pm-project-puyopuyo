#include "Light.h"

Light::Light(float x, float y, float z, int LID)
	: lightID(LID) {
	position[0] = x;
	position[1] = y;
	position[2] = z;
	ambient[0] = ambient[1] = ambient[2] = 0.2f;
	diffuse[0] = diffuse[1] = diffuse[2] = 0.2f;
	specular[0] = specular[1] = specular[2] = 0.5f;
	ambient[3] = diffuse[3] = specular[3] = 1.0f;
}

const int Light::getLightID() const {
	return lightID;
}

void Light::setPos(float x, float y, float z) {
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

const float* Light::getPos() const {
	float pos[] = { position[0], position[1], position[2] };

	return pos;
}

void Light::draw() const {
	float pos[] = { position[0], position[1], position[2], 0 };
	glLightfv(lightID, GL_AMBIENT, ambient);
	glLightfv(lightID, GL_DIFFUSE, diffuse);
	glLightfv(lightID, GL_SPECULAR, specular);
	glLightfv(lightID, GL_POSITION, pos);
}