#include "Board.h"
#include <queue>
#include <vector>

using namespace std;

Board::Board(int ps) : arrangedPuyos(), pixelSize(ps), score(0), combo(0), stage(1) {
	dx[0] = 0; dx[1] = 0; dx[2] = -1; dx[3] = 1;
	dy[0] = 1; dy[1] = -1; dy[2] = 0; dy[3] = 0;
}

void Board::setPuyo(const Puyo& puyo) {
	pair<int, int> idx = getPuyoIndex(puyo);

	arrangedPuyos[idx.first][idx.second] = puyo;
}

void Board::setStage(int s) {
	stage = s;
}

void Board::setCombo(int c) {
	combo = c;
}

void Board::nextStage() {
	stage++;
}

void Board::addScore(int point) {
	score += point;
}

void Board::addCombo() {
	combo++;
}

Puyo& Board::getPuyo(int row, int col) {
	return arrangedPuyos[row][col];
}

int Board::getStage() const {
	return stage;
}

int Board::getScore() const {
	return score;
}

int Board::getCombo() const {
	return combo;
}

pair<int, int> Board::getPuyoIndex(const Puyo& puyo) const {
	const float zeroPos[] = { -3 * pixelSize + puyo.getRadius(), 6 * pixelSize - puyo.getRadius() };
	int row = (zeroPos[1] - puyo.getCenter()[1]) / pixelSize;
	int col = (puyo.getCenter()[0] - zeroPos[0]) / pixelSize;

	return make_pair(row, col);
}

bool Board::hasPuyoToRemove() {
	bool ret = false;

	for (int row = 0; row < 12; row++)
		for (int col = 0; col < 6; col++)
			arrangedPuyos[row][col].setVisited(false);

	for (int row = 11; row >= 0; row--) {
		for (int col = 0; col < 6; col++) {
			Puyo& p = arrangedPuyos[row][col];
			if (!p.getVisited() && p.getColor() != Puyo::Color::NONE) {
				queue<Puyo> q;
				q.push(p);
				p.setVisited(true);

				vector<pair<int, int> > idxVec;
				idxVec.push_back(make_pair(row, col));

				int count = 0;
				while (!q.empty()) {
					int y = getPuyoIndex(q.front()).first;
					int x = getPuyoIndex(q.front()).second;
					Puyo::Color color = q.front().getColor();
					q.pop();
					for (int i = 0; i < 4; i++) {
						int ny = y + dy[i];
						int nx = x + dx[i];

						if (ny >= 0 && ny < 12 && nx >= 0 && nx < 6) {
							Puyo& np = arrangedPuyos[ny][nx];
							if (!np.getVisited() && np.getColor() == color) {
								q.push(np);
								idxVec.push_back(make_pair(ny, nx));
								np.setVisited(true);
							}
						}
					}
				}

				if (idxVec.size() >= 4) {
					int point = 0;
					for (auto& idx : idxVec) {
						arrangedPuyos[idx.first][idx.second].setRemoved(true);
						point += 100;
					}

					combo++;
					score += combo * point;
					ret = true;
				}
			}
		}
	}

	return ret;
}

void Board::clear() {
	for (int row = 0; row < 12; row++) {
		for (int col = 0; col < 6; col++) {
			arrangedPuyos[row][col] = Puyo();
		}
	}

	score = 0;
	combo = 0;
}