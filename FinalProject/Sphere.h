#pragma once

#include "Material.h"
#include "GL/glut.h"

class Sphere {
public:
	Sphere(float r, int sl, int st);
	void setRadius(float r);
	float getRadius() const;
	void setSlice(int sl);
	void setStack(int st);
	void setColor(int cl);
	const int getColor() const;
	void setCenter(float x, float y, float z);
	const float* getCenter() const;
	void setVelocity(float x, float y, float z);
	const float* getVelocity() const;
	void setMTL(const Material& m);
	void move();
	void manualMove(int key, int pixelSize);
	void draw() const;

private:
	float radius;
	int slice;
	int stack;
	int color;
	float center[3];
	float velocity[3];
	Material mtl;
};