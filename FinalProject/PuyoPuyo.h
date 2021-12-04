#pragma once

#include "Puyo.h"

class PuyoPuyo : public Puyo {
public:
	enum class Relation {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	PuyoPuyo();
	PuyoPuyo(const Puyo& sph1, const Puyo& sph2);
	void setPuyos(const Puyo& sph1, const Puyo& sph2);
	void setRelation(Relation r);
	Relation getRelation() const;

	void move();
	void manualMove(int key, int pixelSize);
	void draw() const;
	void clear();

	Puyo& operator[] (int i);

private:
	Puyo puyo[2];
	bool collisionDetected;
	Relation relation;
};