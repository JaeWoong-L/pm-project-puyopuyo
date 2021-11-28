#include "PuyoPuyo.h"

PuyoPuyo::PuyoPuyo() : puyo1(), puyo2(), collisionDetected(false) {}

PuyoPuyo::PuyoPuyo(const Puyo& sph1, const Puyo& sph2) : collisionDetected(false) {
	puyo1 = sph1;
	puyo2 = sph2;
}

void PuyoPuyo::setPuyos(const Puyo& sph1, const Puyo& sph2) {
	puyo1 = sph1;
	puyo2 = sph2;
}

Puyo& PuyoPuyo::getFirstPuyo() {
	return puyo1;
}

Puyo& PuyoPuyo::getSecondPuyo() {
	return puyo2;
}

const int PuyoPuyo::getRelation() const {
	if (puyo1.getCenter()[0] == puyo2.getCenter()[0]) {
		if (puyo1.getCenter()[1] > puyo2.getCenter()[1])
			return UP;
		else
			return DOWN;
	}
	else {
		if (puyo1.getCenter()[0] < puyo2.getCenter()[0])
			return LEFT;
		else
			return RIGHT;
	}
}

void PuyoPuyo::move() {
	puyo1.move();
	puyo2.move();
}

void PuyoPuyo::manualMove(int key, int pixelSize) {
	puyo1.manualMove(key, pixelSize);
	puyo2.manualMove(key, pixelSize);
}

void PuyoPuyo::draw() const {
	puyo1.draw();
	puyo2.draw();
}
