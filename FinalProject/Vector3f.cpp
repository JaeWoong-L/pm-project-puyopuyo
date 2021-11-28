#include "Vector3f.h"

Vector3f::Vector3f() {
	pos[0] = pos[1] = pos[2] = 0.0f;
}

Vector3f::Vector3f(float x, float y, float z) {
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}

void Vector3f::setPos(float x, float y, float z) {
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
}

float& Vector3f::operator[](const int i) {
	return pos[i];
}

float Vector3f::operator[](const int i) const {
	return pos[i];
}

Vector3f& Vector3f::operator+=(const Vector3f& v) {
	pos[0] += v.pos[0];
	pos[1] += v.pos[1];
	pos[2] += v.pos[2];
	
	return (*this);
}

Vector3f Vector3f::operator-(const Vector3f& v) {
	return Vector3f(pos[0] - v.pos[0], pos[1] - v.pos[1], pos[2] - v.pos[2]);
}

Vector3f Vector3f::operator*(const float s) {
	return Vector3f(s * pos[0], s * pos[1], s * pos[2]);
}

bool Vector3f::operator==(const Vector3f& v) {
	return (pos[0] == v.pos[0]) && (pos[1] == v.pos[1]) && (pos[2] == v.pos[2]);
}

float dotProduct(Vector3f v1, Vector3f v2) {
	float ans = 0.0f;
	for (int i = 0; i < 3; i++)
		ans += v1[i] * v2[i];

	return ans;
}