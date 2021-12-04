#pragma once

#include <utility>
#include "Puyo.h"

using namespace std;

class Board {
public:
	Board(int ps);

	void setPuyo(const Puyo& puyo);
	void setStage(int s);
	void setCombo(int c);
	void nextStage();
	void addScore(int sc);
	void addCombo();
	
	Puyo& getPuyo(int row, int col);
	int getStage() const;
	int getScore() const;
	int getCombo() const;

	pair<int, int> getPuyoIndex(const Puyo& puyo) const;
	bool hasPuyoToRemove();
	void clear();

private:
	Puyo arrangedPuyos[12][6];
	int pixelSize;
	int score;
	int combo;
	int stage;
	int dx[4];
	int dy[4];
};