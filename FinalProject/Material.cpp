#include "Material.h"

Material::Material() {
	setEmission(Vector4f(0.05f, 0.05f, 0.05f, 1.0f));
	setAmbient(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	setDiffuse(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	setSpecular(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	setShininess(50.0);
}

void Material::setEmission(const Vector4f& em) {
	emission = em;
}

void Material::setAmbient(const Vector4f& am) {
	ambient = am;
}

void Material::setDiffuse(const Vector4f& di) {
	diffuse = di;
}

void Material::setSpecular(const Vector4f& sp) {
	specular = sp;
}

void Material::setShininess(float sh) {
	shininess = sh;
}

Vector4f Material::getEmission() const {
	return emission;
}

Vector4f Material::getAmbient() const {
	return ambient;
}

Vector4f Material::getDiffuse() const {
	return diffuse;
}

Vector4f Material::getSpecular() const {
	return specular;
}

float Material::getShininess() const {
	return shininess;
}