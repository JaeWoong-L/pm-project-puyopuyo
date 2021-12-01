#include "PuyoPuyo.h"

PuyoPuyo::PuyoPuyo() : puyo{}, collisionDetected(false), relation(Relation::LEFT) {}

PuyoPuyo::PuyoPuyo(const Puyo& sph1, const Puyo& sph2) : collisionDetected(false), relation(Relation::LEFT) {
	puyo[0] = sph1;
	puyo[1] = sph2;
}

void PuyoPuyo::setPuyos(const Puyo& sph1, const Puyo& sph2) {
	puyo[0] = sph1;
	puyo[1] = sph2;
}

void PuyoPuyo::setRelation(Relation r) {
	relation = r;
}

//Relation PuyoPuyo::getRelation() const {
	//if (puyo[0].getCenter()[0] == puyo[1].getCenter()[0]) {
	//	if (puyo[0].getCenter()[1] > puyo[1].getCenter()[1])
	//		return UP;
	//	else
	//		return DOWN;
	//}
	//else {
	//	if (puyo[0].getCenter()[0] < puyo[1].getCenter()[0])
	//		return LEFT;
	//	else
	//		return RIGHT;
	//}
//}

PuyoPuyo::Relation PuyoPuyo::getRelation() const {
	return relation;
}

void PuyoPuyo::move() {
	puyo[0].move();
	puyo[1].move();
}

void PuyoPuyo::manualMove(int key, int pixelSize) {
	puyo[0].manualMove(key, pixelSize);
	puyo[1].manualMove(key, pixelSize);
}

void PuyoPuyo::draw() const {
	puyo[0].draw();
	puyo[1].draw();
}

Puyo& PuyoPuyo::operator[](int i) {
	return puyo[i];
}