#pragma once

#include "Vector3f.h"
#include "Vector4f.h"
#include "GL/glut.h"

class Light {
public:
	Light(float x, float y, float z, int lightID);
	const int getLightID() const;
	void setPosition(const Vector3f& pos);
	Vector3f getPosition() const;
	void draw() const;
	
private:
	int lightID;
	Vector3f position;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
};