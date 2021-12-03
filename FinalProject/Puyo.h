#pragma once

#include "Material.h"
#include "Vector3f.h"
#include "GL/glut.h"

class Puyo {
public:
	Puyo();
	Puyo(float r, int sl, int st);
	void setRadius(float r);
	void setColor(int cl);
	void setCenter(const Vector3f& c);
	void setVelocity(const Vector3f& v);
	void setMTL(const Material& m);
	void setVisited(bool v);
	void setRemoved(bool r);
	void fadeOut(int maxCount);

	float getRadius() const;
	int getColor() const;
	Vector3f getCenter() const;
	Vector3f getVelocity() const;
	const Material& getMaterial() const;
	bool getVisited() const;
	bool getRemoved() const;

	virtual void move();
	virtual void manualMove(int key, int pixelSize);
	virtual void draw() const;

	bool operator&&(const Puyo& sph) const;

private:
	float radius;
	int slice;
	int stack;
	int color;
	Vector3f center;
	Vector3f velocity;
	Material mtl;
	bool visited;
	bool removed;
};