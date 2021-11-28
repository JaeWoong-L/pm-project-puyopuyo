#pragma once

#include "Puyo.h"

class PuyoPuyo : public Puyo {
public:
	enum relation {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	PuyoPuyo();
	PuyoPuyo(const Puyo& sph1, const Puyo& sph2);
	void setPuyos(const Puyo& sph1, const Puyo& sph2);
	Puyo& getFirstPuyo();
	Puyo& getSecondPuyo();
	const int getRelation() const;

	void move();
	void manualMove(int key, int pixelSize);
	void draw() const;

private:
	Puyo puyo1, puyo2;
	bool collisionDetected;
};