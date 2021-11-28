#include <cmath>
#include "Puyo.h"

Puyo::Puyo() : radius(0), slice(0), stack(0), color(0), velocity(), mtl() {
	center[0] = center[1] = center[2] = 1000;
}

Puyo::Puyo(float r, int sl, int st) : color(0), center(), velocity(), mtl() {
	radius = r;
	slice = sl;
	stack = st;
}

void Puyo::setRadius(float r) {
	radius = r;
}

void Puyo::setColor(int cl) {
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

float Puyo::getRadius() const {
	return radius;
}

int Puyo::getColor() const {
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
		center[1] -= pixelSize / 2;
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
	glutSolidPuyo(radius, slice, stack);

	glPopMatrix();
}

bool Puyo::operator&&(const Puyo& sph) const {
	float distance = 0;
	for (int i = 0; i < 3; i++)
		distance += pow(center[i] - sph.center[i], 2);

	return (pow(radius + sph.radius, 2) >= distance);
}