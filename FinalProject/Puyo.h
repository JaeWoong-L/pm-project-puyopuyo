#pragma once

#include "Material.h"
#include "Vector3f.h"
#include "GL/glut.h"

class Puyo {
public:
	enum class Color {
		NONE,
		RED,
		GREEN,
		BLUE,
		YELLOW,
		PURPLE
	};

	Puyo();
	Puyo(float r, int sl, int st);
	void setRadius(float r);
	void setColor(Color cl);
	void setCenter(const Vector3f& c);
	void setVelocity(const Vector3f& v);
	void setMTL(const Material& m);
	void setVisited(bool v);
	void setRemoved(bool r);
	void fadeOut(int maxCount);

	float getRadius() const;
	Color getColor() const;
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
	Color color;
	Vector3f center;
	Vector3f velocity;
	Material mtl;
	bool visited;
	bool removed;
};