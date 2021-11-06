#include <iostream>
#include "Texture.h"

using namespace std;

Texture::Texture() : textureID(0), textureData(nullptr), imageWidth(0), imageHeight(0) {}

FIBITMAP* Texture::createBitMap(char const* filename) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	if (format == -1) {
		cout << "Could not find image: " << filename << " - Aborting." << endl;
		exit(-1);
	}

	if (format == FIF_UNKNOWN) {
		cout << "Couldn't determine file format - attempting to get from file extension..." << endl;
		format = FreeImage_GetFIFFromFilename(filename);

		if (!FreeImage_FIFSupportsReading(format)) {
			cout << "Detected image format cannot be read!" << endl;
			exit(-1);
		}
	}
	
	FIBITMAP* bitmap = FreeImage_Load(format, filename);

	int bitsPerPixel = FreeImage_GetBPP(bitmap);

	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32) {
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << endl;
		bitmap32 = bitmap;
	}
	else {
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << endl;
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}

	return bitmap32;
}

void Texture::generateTexture() {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, textureData);
}

void Texture::initializeTexture(char const* filename) {
	FIBITMAP* bitmap32 = createBitMap(filename);
	imageWidth = FreeImage_GetWidth(bitmap32);
	imageHeight = FreeImage_GetHeight(bitmap32);
	textureData = FreeImage_GetBits(bitmap32);

	generateTexture();
}

GLuint Texture::getTextureID() const {
	return textureID;
}

void Texture::drawSquareWithTexture(float x1, float y1, float x2, float y2) {
	/* Implement: Bind texture */
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(x1, y1);
	glTexCoord2f(0, 1); glVertex2f(x1, y2);
	glTexCoord2f(1, 1); glVertex2f(x2, y2);
	glTexCoord2f(1, 0); glVertex2f(x2, y1);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}