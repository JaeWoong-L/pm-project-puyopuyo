#pragma once

#include "FreeImage.h"
#include "GL/glut.h"

class Texture {
public:
	Texture();
	FIBITMAP* createBitMap(char const* filename);
	void generateTexture();
	void initializeTexture(char const* filename);
	GLuint getTextureID() const;
	void drawSquareWithTexture(float x1, float y1, float x2, float y2);

private:
	GLuint textureID;
	GLubyte* textureData;
	int imageWidth, imageHeight;
};