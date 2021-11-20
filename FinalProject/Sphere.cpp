#include "Sphere.h"

Sphere::Sphere(float r, int sl, int st) : color(0), center(), velocity(), mtl() {
	radius = r;
	slice = sl;
	stack = st;
}

void Sphere::setRadius(float r) {
	radius = r;
}

float Sphere::getRadius() const {
	return radius;
}

void Sphere::setSlice(int sl) {
	slice = sl;
}

void Sphere::setStack(int st) {
	stack = st;
}

void Sphere::setColor(int cl) {
	color = cl;
}

const int Sphere::getColor() const {
	return color;
}

void Sphere::setCenter(float x, float y, float z) {
	center[0] = x;
	center[1] = y;
	center[2] = z;
}

const float* Sphere::getCenter() const {
	return center;
}

void Sphere::setVelocity(float x, float y, float z) {
	velocity[0] = x;
	velocity[1] = y;
	velocity[2] = z;
}

const float* Sphere::getVelocity() const {
	return velocity;
}

void Sphere::setMTL(const Material& m) {
	mtl = m;
}

void Sphere::move() {
	center[0] += velocity[0];
	center[1] += velocity[1];
	center[2] += velocity[2];
}

void Sphere::manualMove(int key, int pixelSize) {
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

void Sphere::draw() const {
	glPushMatrix();

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_EMISSION, mtl.getEmission());
	glMaterialfv(GL_FRONT, GL_AMBIENT, mtl.getAmbient());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mtl.getDiffuse());
	glMaterialfv(GL_FRONT, GL_SPECULAR, mtl.getSpecular());
	glMaterialfv(GL_FRONT, GL_SHININESS, mtl.getShininess());
	glTranslatef(center[0], center[1], center[2]);
	glutSolidSphere(radius, slice, stack);

	glPopMatrix();
}