#pragma once

#include "GL/glut.h"

class Board {
public:
	Board(int ps);
	void draw(const int width, const int height) const;
	void remove();
	void addScore(int sc);
	const int getScore() const;

private:
	char board[12][6];
	bool visited[12][6];
	int pixelSize;
	float margin;
	int score;
};