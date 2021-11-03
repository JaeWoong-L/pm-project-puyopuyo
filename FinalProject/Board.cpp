//#define DEBUG 1
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

#include "Board.h"

Board::Board(int ps) : score(0) {
	pixelSize = ps;
	margin = (float)ps / 20;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 5; j++) {
			board[i][j] = '0';
			visited[i][j] = false;
		}
	}
}

void Board::draw(const int width, const int height) const {
	#ifdef DEBUG
		cout << "board draw" << endl;
		glColor3f(1, 0, 0);
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex3f(-160, 300, 0);
		glVertex3f(-160, -300, 0);
		glVertex3f(160, 300, 0);
		glVertex3f(160, -300, 0);
		glEnd();
	#endif

	glPushMatrix();

	glTranslatef(-width/2 + pixelSize, height/2 - pixelSize, 0);
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 6; j++) {
			
			glPushMatrix();
			
			if ((i + j) % 2 == 0)
				glColor3f(0.2f, 0.2f, 0.2f);
			else
				glColor3f(0.3f, 0.3f, 0.3f);
			glTranslatef(j * pixelSize, (-i) * pixelSize, 0);
			glBegin(GL_QUADS);
			glVertex3f(margin, -margin, 0);
			glVertex3f(margin, margin - pixelSize, 0);
			glVertex3f(pixelSize - margin, margin -pixelSize, 0);
			glVertex3f(pixelSize - margin, -margin, 0);
			glEnd();

			glPopMatrix();
		}
	}

	glPopMatrix();
}

void Board::remove() {

}

void Board::addScore(int point) {
	score += point;
}

const int Board::getScore() const {
	return score;
}