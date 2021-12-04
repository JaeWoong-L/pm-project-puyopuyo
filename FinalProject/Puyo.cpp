#include <cmath>
#include "Puyo.h"

Puyo::Puyo() : radius(0), slice(0), stack(0), color(Color::NONE), velocity(), mtl(), visited(false), removed(false) {
	center[0] = center[1] = center[2] = 1000;
}

Puyo::Puyo(float r, int sl, int st) : color(Color::NONE), center(), velocity(), mtl(), visited(false), removed(false) {
	radius = r;
	slice = sl;
	stack = st;
}

void Puyo::setRadius(float r) {
	radius = r;
}

void Puyo::setColor(Color cl) {
	color = cl;
}

void Puyo::setCenter(const Vector3f& c) {
	center = c;
}

void Puyo::setVelocity(const Vector3f& v) {
	velocity = v;
}

void Puyo::setMTL(const Material& m) {
	mtl = m;
}

void Puyo::setVisited(bool v) {
	visited = v;
}

void Puyo::setRemoved(bool r) {
	removed = r;
}

void Puyo::fadeOut(int maxCount) {
	Vector4f am = mtl.getAmbient();
	Vector4f di = mtl.getDiffuse();
	Vector4f sp = mtl.getSpecular();

	float subtractor = 1.0f / maxCount;
	for (int i = 0; i < 3; i++) {
		am[i] -= subtractor;
		di[i] -= subtractor;
		sp[i] -= subtractor;
	}

	mtl.setAmbient(am);
	mtl.setDiffuse(di);
	mtl.setSpecular(sp);
}

float Puyo::getRadius() const {
	return radius;
}

Puyo::Color Puyo::getColor() const {
	return color;
}

Vector3f Puyo::getCenter() const {
	return center;
}

Vector3f Puyo::getVelocity() const {
	return velocity;
}

const Material& Puyo::getMaterial() const {
	return mtl;
}

bool Puyo::getVisited() const {
	return visited;
}

bool Puyo::getRemoved() const {
	return removed;
}

void Puyo::move() {
	center += velocity;
}

void Puyo::manualMove(int key, int pixelSize) {
	switch (key) {
	case GLUT_KEY_LEFT:
		center[0] -= pixelSize;
		break;
	case GLUT_KEY_RIGHT:
		center[0] += pixelSize;
		break;
	case GLUT_KEY_DOWN:
		center[1] -= pixelSize;
		break;
	default:
		break;
	}
}

void Puyo::draw() const {
	Vector4f emission = mtl.getEmission();
	Vector4f ambient = mtl.getAmbient();
	Vector4f diffuse = mtl.getDiffuse();
	Vector4f specular = mtl.getSpecular();

	float em[] = { emission[0], emission[1], emission[2], emission[3] };
	float am[] = { ambient[0], ambient[1], ambient[2], ambient[3] };
	float di[] = { diffuse[0], diffuse[1], diffuse[2], diffuse[3] };
	float sp[] = { specular[0], specular[1], specular[2], specular[3] };
	float sh[] = { mtl.getShininess() };

	glPushMatrix();

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_EMISSION, em);
	glMaterialfv(GL_FRONT, GL_AMBIENT, am);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, di);
	glMaterialfv(GL_FRONT, GL_SPECULAR, sp);
	glMaterialfv(GL_FRONT, GL_SHININESS, sh);
	glTranslatef(center[0], center[1], center[2]);
	glutSolidSphere(radius, slice, stack);

	glPopMatrix();
}

bool Puyo::operator&&(const Puyo& sph) const {
	float distance = 0;
	for (int i = 0; i < 3; i++)
		distance += pow(center[i] - sph.center[i], 2);

	if (distance == 0)
		return false;

	return (pow(radius + sph.radius, 2) >= distance);
}