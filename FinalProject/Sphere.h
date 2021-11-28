#pragma once

#include "Material.h"
#include "Vector3f.h"
#include "GL/glut.h"

class Sphere {
public:
	Sphere();
	Sphere(float r, int sl, int st);
	void setRadius(float r);
	void setColor(int cl);
	void setCenter(const Vector3f& c);
	void setVelocity(const Vector3f& v);
	void setMTL(const Material& m);

	float getRadius() const;
	int getColor() const;
	Vector3f getCenter() const;
	Vector3f getVelocity() const;
	const Material& getMaterial() const;

	void move();
	void manualMove(int key, int pixelSize);
	void draw() const;

	bool operator&&(const Sphere& sph) const;

private:
	float radius;
	int slice;
	int stack;
	int color;
	Vector3f center;
	Vector3f velocity;
	Material mtl;
};