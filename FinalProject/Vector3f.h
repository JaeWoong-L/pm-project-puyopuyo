#pragma once

class Vector3f {
public:
	Vector3f();
	Vector3f(float x, float y, float z);
	void setPos(float x, float y, float z);
	float& operator[](const int i);
	float operator[](const int i) const;
	Vector3f& operator+=(const Vector3f& v);
	Vector3f operator-(const Vector3f& v);
	Vector3f operator*(const float s);
	bool operator==(const Vector3f& v);

private:
	float pos[3];
};

float dotProduct(Vector3f v1, Vector3f v2);