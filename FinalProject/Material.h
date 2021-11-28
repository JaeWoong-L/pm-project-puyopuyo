#pragma once

#include "Vector4f.h"

class Material {
public:
	Material();
	void setEmission(const Vector4f& em);
	void setAmbient(const Vector4f& am);
	void setDiffuse(const Vector4f& di);
	void setSpecular(const Vector4f& sp);
	void setShininess(float sh);
	Vector4f getEmission() const;
	Vector4f getAmbient() const;
	Vector4f getDiffuse() const;
	Vector4f getSpecular() const;
	float getShininess() const;

private:
	Vector4f emission;
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	float shininess;
};