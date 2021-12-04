#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "Light.h"
#include "Puyo.h"
#include "PuyoPuyo.h"
#include "Board.h"
#include "Texture.h"

using namespace std;

// 8:15 비율 8:5  1080 : 675
#define WIDTH 1080
#define HEIGHT 675
#define PIXEL_SIZE 40 // 한 칸 : 40
#define RADIUS PIXEL_SIZE/2 // 반지름: 20

#define boundaryX WIDTH/2
#define boundaryY HEIGHT/2
#define boardBoundaryX PIXEL_SIZE*3
#define boardBoundaryY PIXEL_SIZE*6

#define UP PuyoPuyo::Relation::UP
#define DOWN PuyoPuyo::Relation::DOWN
#define LEFT PuyoPuyo::Relation::LEFT
#define RIGHT PuyoPuyo::Relation::RIGHT

#define PI 3.141592
#define SEG 180

enum Color {
	NONE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	PURPLE
};

enum Scene {
	START,
	PLAY,
	PAUSE,
	VICTORY,
	DEFEAT
};

Scene scene = START;
Board board(PIXEL_SIZE);
PuyoPuyo startPuyo, nextPuyo;
Puyo arrangedPuyos[12][6];
vector<Puyo> dropPuyos;
Puyo emptyPuyo;
Vector3f dropVelocity(0, -2, 0);
Vector3f zeroVector3f;
int dx[4] = { 0, 0, -1, 1 };
int dy[4] = { 1, -1, 0, 0 };
int fadeOutCount = 200;

Light light(boundaryX/2, boundaryX, boundaryX, GL_LIGHT0);
Material red, green, blue, yellow, purple;
Texture titleTexture, titleTexture2, backgroundTexture, boardTexture, frameTexture, keyTexture;

clock_t start_t = clock(), end_t;
clock_t blinkStart_t = clock(), blinkEnd_t;
bool isIndicatorOn = true;
bool keyboardValidator = true;
bool removed = false;
bool removeFinished = false;

// function declarations
void generateRandomColor(Puyo& s, int max);
void drawIndicator(const Vector3f& center);
void remove();
void rotate();
bool collisionDetectionBottom(const Puyo& puyo);
bool collisionDetectionLeft(const Puyo& puyo);
bool collisionDetectionRight(const Puyo& puyo);
void collisionHandling();
void arrangePuyo(const Puyo& puyo);
void createPuyo();
void initialize();
void idle();
void displayCharacters(void* font, float color[3], string str, float x, float y);
void display();
void keyboardDown(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void reshape(int w, int h);

/*--------------------------------------------------------------
----------------------------------------------------------------
----------------------- Main Function --------------------------
----------------------------------------------------------------
--------------------------------------------------------------*/
int main(int argc, char** argv) {
	srand(time(NULL));

	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(650, 300);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("PuyoPuyo 2");
	initialize();

	// register callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(specialKeyDown);
	if(scene != PAUSE)
		glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}

// function definitions
void generateRandomColor(Puyo &s, int max) {
	int color = rand() % max + 1;
	switch (color) {
	case RED:
		s.setMTL(red);
		s.setColor(RED);
		break;
	case GREEN:
		s.setMTL(green);
		s.setColor(GREEN);
		break;
	case BLUE:
		s.setMTL(blue);
		s.setColor(BLUE);
		break;
	case YELLOW:
		s.setMTL(yellow);
		s.setColor(YELLOW);
		break;
	case PURPLE:
		s.setMTL(purple);
		s.setColor(PURPLE);
		break;
	default:
		break;
	}
}

void drawIndicator(const Vector3f& center) {
	/* Implement: draw all circles */
	glColor3f(0.7f, 0.7f, 0.7f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= SEG; i++) {
		float angle = i * (2 * PI / SEG);
		float dx = (RADIUS + 0.5) * cos(angle);
		float dy = (RADIUS + 0.5) * sin(angle);
		glVertex3f(center[0] + dx, center[1] + dy, center[2] + 1);
	}
	glEnd();
}

bool collisionDetectionBottom(const Puyo& puyo) {
	// between puyos
	for(int row = 0; row < 12; row++)
		for(int col = 0; col < 6; col++)
			if (arrangedPuyos[row][col].getColor() != NONE
				&& (puyo && arrangedPuyos[row][col])
				&& puyo.getCenter()[0] == arrangedPuyos[row][col].getCenter()[0]
				&& puyo.getCenter()[1] > arrangedPuyos[row][col].getCenter()[1])
				return true;

	for (const Puyo& dropPuyo : dropPuyos)
		if (((puyo && dropPuyo) && dropPuyo.getVelocity() == zeroVector3f)
			&& puyo.getCenter()[0] == dropPuyo.getCenter()[0]
			&& puyo.getCenter()[1] > dropPuyo.getCenter()[1])
			return true;

	// lower boundary
	if (puyo.getCenter()[1] - RADIUS <= -boardBoundaryY)
		return true;

	return false;
}

bool collisionDetectionLeft(const Puyo& puyo) {
	// between puyos
	for (int row = 0; row < 12; row++)
		for (int col = 0; col < 6; col++)
			if (arrangedPuyos[row][col].getColor() != NONE
				&& (puyo && arrangedPuyos[row][col])
				&& puyo.getCenter()[0] > arrangedPuyos[row][col].getCenter()[0])
				return true;

	// lower boundary
	if (puyo.getCenter()[0] - RADIUS <= -boardBoundaryX)
		return true;

	return false;
}

bool collisionDetectionRight(const Puyo& puyo) {
	// between puyos
	for (int row = 0; row < 12; row++)
		for (int col = 0; col < 6; col++)
			if (arrangedPuyos[row][col].getColor() != NONE
				&& (puyo && arrangedPuyos[row][col])
				&& puyo.getCenter()[0] < arrangedPuyos[row][col].getCenter()[0])
				return true;

	// lower boundary
	if (puyo.getCenter()[0] + RADIUS >= boardBoundaryX)
		return true;

	return false;
}

void collisionHandling() {
	if (collisionDetectionBottom(startPuyo[0])) {
		startPuyo[0].setVelocity(zeroVector3f);

		if (startPuyo.getRelation() == UP || startPuyo.getRelation() == DOWN)
			startPuyo[1].setVelocity(zeroVector3f);
	}
	if (collisionDetectionBottom(startPuyo[1])) {
		startPuyo[1].setVelocity(zeroVector3f);

		if (startPuyo.getRelation() == UP || startPuyo.getRelation() == DOWN)
			startPuyo[0].setVelocity(zeroVector3f);
	}
}

pair<int, int> getBoardIndex(const Vector3f& pos) {
	const float zeroPos[] = { -3 * PIXEL_SIZE + RADIUS, 6 * PIXEL_SIZE - RADIUS };
	int row = (zeroPos[1] - pos[1]) / PIXEL_SIZE;
	int col = (pos[0] - zeroPos[0]) / PIXEL_SIZE;

	return make_pair(row, col);
}

void arrangePuyo(const Puyo& puyo) {
	pair<int, int> boardIndex = getBoardIndex(puyo.getCenter());

	arrangedPuyos[boardIndex.first][boardIndex.second] = puyo;
}

void remove() {
	removed = false;
	for (int row = 0; row < 12; row++)
		for (int col = 0; col < 6; col++)
			arrangedPuyos[row][col].setVisited(false);

	for (int row = 11; row >= 0; row--) {
		for (int col = 0; col < 6; col++) {
			Puyo& p = arrangedPuyos[row][col];
			if (!p.getVisited() && p.getColor() != NONE) {
				queue<Puyo> q;
				q.push(p);
				p.setVisited(true);

				vector<pair<int, int> > idxVec;
				idxVec.push_back(make_pair(getBoardIndex(p.getCenter()).first, getBoardIndex(p.getCenter()).second));

				int count = 0;
				while (!q.empty()) {
					int y = getBoardIndex(q.front().getCenter()).first;
					int x = getBoardIndex(q.front().getCenter()).second;
					int color = q.front().getColor();
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
					for (auto& idx : idxVec) {
						cout << "제거할 뿌요들: ";
						cout << idx.first << ", " << idx.second << endl;
						arrangedPuyos[idx.first][idx.second].setRemoved(true);
					}

					removed = true;
				}
			}
		}
	}
}

void rotate() {
	Vector3f secondPuyoPos = startPuyo[1].getCenter();

	switch (startPuyo.getRelation()) {
	case UP:
		if (collisionDetectionLeft(startPuyo[1])) {
			if (collisionDetectionRight(startPuyo[1])) {
				Vector3f firstPuyoPos = startPuyo[0].getCenter();
				startPuyo[0].setCenter(secondPuyoPos);
				startPuyo[1].setCenter(firstPuyoPos);
				startPuyo.setRelation(DOWN);
			}
			else {
				startPuyo[0].setCenter(secondPuyoPos);
				startPuyo[1].setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
				startPuyo.setRelation(LEFT);
			}
		}
		else {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
			startPuyo.setRelation(LEFT);
		}
		break;
	case DOWN:
		if (collisionDetectionRight(startPuyo[1])) {
			if(collisionDetectionLeft(startPuyo[1])) {
				Vector3f firstPuyoPos = startPuyo[0].getCenter();
				startPuyo[0].setCenter(secondPuyoPos);
				startPuyo[1].setCenter(firstPuyoPos);
				startPuyo.setRelation(UP);
			}
			else {
				startPuyo[0].setCenter(secondPuyoPos);
				startPuyo[1].setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
				startPuyo.setRelation(RIGHT);
			}
		}
		else {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
			startPuyo.setRelation(RIGHT);
		}
		break;
	case LEFT:
		if (collisionDetectionBottom(startPuyo[1])) {
			startPuyo[0].setCenter(secondPuyoPos);
			startPuyo[1].setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] + PIXEL_SIZE, 0));
		}
		else {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] - PIXEL_SIZE, 0));
		}
		startPuyo.setRelation(DOWN);
		break;
	case RIGHT:
		startPuyo[0].setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] + PIXEL_SIZE, 0));
		startPuyo.setRelation(UP);
		break;
	default:
		break;
	}
}

void createPuyo() {
	Vector3f startPuyoPos[2] = { Vector3f(-RADIUS, 5 * PIXEL_SIZE + RADIUS, 0),
								 Vector3f(-RADIUS, 5 * PIXEL_SIZE - RADIUS, 0) };
	Vector3f nextPuyoPos[2] = { Vector3f(4.5 * PIXEL_SIZE, 1.5 * PIXEL_SIZE, 0),
								Vector3f(4.5 * PIXEL_SIZE, 0.5 * PIXEL_SIZE, 0) };
	Vector3f initialVelocity(0, -PIXEL_SIZE, 0);

	if (startPuyo[0].getColor() == NONE) {
		Puyo sph1(RADIUS, 20, 20), sph2(RADIUS, 20, 20);

		//init startPuyo
		sph1.setCenter(startPuyoPos[0]);
		sph1.setVelocity(initialVelocity);
		generateRandomColor(sph1, 4);

		sph2.setCenter(startPuyoPos[1]);
		sph2.setVelocity(initialVelocity);
		generateRandomColor(sph2, 4);

		startPuyo.setPuyos(sph1, sph2);

		//init nextPuyo
		sph1.setCenter(nextPuyoPos[0]);
		generateRandomColor(sph1, 4);

		sph2.setCenter(nextPuyoPos[1]);
		generateRandomColor(sph2, 4);

		nextPuyo.setPuyos(sph1, sph2);
	}
	else {
		startPuyo = nextPuyo;
		startPuyo[0].setCenter(startPuyoPos[0]);
		startPuyo[1].setCenter(startPuyoPos[1]);

		generateRandomColor(nextPuyo[0], 4);
		generateRandomColor(nextPuyo[1], 4);
	}
}

void initialize() {
	//init materials
	red.setAmbient(Vector4f(1, 0, 0, 0));
	green.setAmbient(Vector4f(0, 1, 0, 0));
	blue.setAmbient(Vector4f(0, 0, 1, 0));
	yellow.setAmbient(Vector4f(1, 1, 0, 0));
	purple.setAmbient(Vector4f(1, 0, 1, 0));

	//init textures
	string titleImg = "title.png";
	titleTexture.initializeTexture(titleImg.c_str());
	
	string titleImg2 = "title2.png";
	titleTexture2.initializeTexture(titleImg2.c_str());

	string backgroundImg = "background_christmas_cartoon.jpg";
	backgroundTexture.initializeTexture(backgroundImg.c_str());

	//string boardImg = "background_board.png";
	//boardTexture.initializeTexture(boardImg.c_str());

	string frameImg = "frame_brown.png";
	frameTexture.initializeTexture(frameImg.c_str());

	string keyImg = "arrow_key.png";
	keyTexture.initializeTexture(keyImg.c_str());

	createPuyo();
}

void setDropPuyos() {
	for (int i = 11; i >= 0; i--) {
		for (int j = 0; j < 6; j++) {
			if (arrangedPuyos[i][j].getColor() != NONE) {
				arrangedPuyos[i][j].setVelocity(dropVelocity);
				dropPuyos.push_back(arrangedPuyos[i][j]);
				arrangedPuyos[i][j] = emptyPuyo;
			}
			else
				arrangedPuyos[i][j].setVelocity(zeroVector3f);
		}
	}

	cout << "drop puyo count: " << dropPuyos.size() << endl;
}

void rearrangeBoard() {
	cout << "rearrange start" << endl;
	vector<Puyo> v;
	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 6; j++)
			if (arrangedPuyos[i][j].getColor() != NONE)
				v.push_back(arrangedPuyos[i][j]);

	cout << "Before rearrange" << endl;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 6; j++) {
			arrangedPuyos[i][j].setColor(NONE);
			cout << arrangedPuyos[i][j].getColor() << " ";
		}
		cout << endl;
	}
	cout << endl;

	for (const Puyo& puyo : v) {
		arrangePuyo(puyo);
	}

	cout << "After rearrange" << endl;
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 6; j++) {
			cout << (Color)arrangedPuyos[i][j].getColor() << " ";
		}
		cout << endl;
	}
	cout << endl;
}

bool drop = false;
bool blinker = true;
void idle() {
	if (scene == START) {
		end_t = clock();
		if (end_t - start_t > CLOCKS_PER_SEC / 2) {
			blinker = !blinker;
			start_t = end_t;
		}
	}
	else if (scene == PLAY) {
		if (arrangedPuyos[0][2].getColor() != NONE) {
			scene = DEFEAT;
		}
		else if (removed) {
			if (drop) {
				cout << "drop start!" << endl;
				//setDropPuyos();
				//if (dropPuyos.empty()) {
				//	for (int i = 11; i >= 0; i--) {
				//		for (int j = 0; j < 6; j++) {
				//			if (arrangedPuyos[i][j].getColor() != NONE) {
				//				if (!collisionDetectionBottom(arrangedPuyos[i][j])) {
				//					cout << "drop puyo's address: ";
				//					cout << i << ", " << j << endl;
				//					dropPuyos.push_back(arrangedPuyos[i][j]);
				//					arrangedPuyos[i][j].setColor(NONE);
				//					/// arrangeedPuyos 인덱스 바꿀 방법 생각해보기
				//					/// readyToDrop 함수에서 뿌요들 vector에 다 집어넣고
				//					/// drop 끝난 이후에 다시 위치맞춰서 arrangedPuyos에 넣어주기.
				//				}
				//			}
				//		}
				//	}
				//}
				if (dropPuyos.empty()) {
					cout << "dropPuyo empty" << endl;
					setDropPuyos();
					for (Puyo& puyo : dropPuyos)
						cout << getBoardIndex(puyo.getCenter()).first << ", " << getBoardIndex(puyo.getCenter()).second << endl;
				}

				cout << "drop ongoing" << endl;
				int moveCount = 0;
				for (Puyo& puyo : dropPuyos) {
					cout << getBoardIndex(puyo.getCenter()).first << ", " << getBoardIndex(puyo.getCenter()).second << endl;
					if (!collisionDetectionBottom(puyo)) {
						puyo.move();
						moveCount++;
					}
					else {
						puyo.setVelocity(zeroVector3f);
					}
				}

				if (moveCount == 0) {
					//rearrangeBoard();
					for (const Puyo& puyo : dropPuyos)
						arrangePuyo(puyo);
					dropPuyos.clear();

					fadeOutCount = 200;
					removed = false;
					drop = false;
					removeFinished = true;
					cout << "drop end" << endl;
				}
			}
			else if (fadeOutCount <= 0) {
				for (int i = 0; i < 12; i++) {
					for (int j = 0; j < 6; j++) {
						if (arrangedPuyos[i][j].getRemoved()) {
							cout << "fadeout puyo's address: ";
							cout << i << ", " << j << endl;
							arrangedPuyos[i][j].setColor(NONE);
						}
					}
				}

				drop = true;
			}
			else {
				isIndicatorOn = false;
				keyboardValidator = false;

				for (int i = 0; i < 12; i++) {
					for (int j = 0; j < 6; j++) {
						if (arrangedPuyos[i][j].getRemoved()) {
							arrangedPuyos[i][j].fadeOut(fadeOutCount);
							fadeOutCount--;
						}
					}
				}
			}
		}
		else {
			//collision handling
			collisionHandling();
			if (startPuyo[0].getVelocity()[1] == 0 && startPuyo[1].getVelocity()[1] == 0) {
				if (!removed && !removeFinished) {
					arrangePuyo(startPuyo[0]);
					arrangePuyo(startPuyo[1]);
					keyboardValidator = true;
					removeFinished = true;
				}

				remove();
				if (!removed && removeFinished) {
					createPuyo();
					keyboardValidator = true;
					removeFinished = false;
				}
			}
			else if (startPuyo[0].getVelocity()[1] == 0) {
				isIndicatorOn = false;
				keyboardValidator = false;

				startPuyo[1].setVelocity(dropVelocity);
				startPuyo[1].move();
			}
			else if (startPuyo[1].getVelocity()[1] == 0) {
				isIndicatorOn = false;
				keyboardValidator = false;

				startPuyo[0].setVelocity(dropVelocity);
				startPuyo[0].move();
			}
			else {
				//blink indicator
				blinkEnd_t = clock();
				if ((blinkEnd_t - blinkStart_t) > CLOCKS_PER_SEC / 6) {
					isIndicatorOn = !isIndicatorOn;

					blinkStart_t = blinkEnd_t;
				}

				//idle move
				end_t = clock();
				if ((end_t - start_t) > CLOCKS_PER_SEC) {
					startPuyo.move();

					start_t = end_t;
				}
			}
		}
	}
	else if (scene == PAUSE) {
		//do nothing
	}

	glutPostRedisplay();
}

void displayCharacters(void* font, float color[3], string str, float x, float y) {
	glColor3f(color[0], color[1], color[2]);
	glRasterPos2f(x, y);
	for (auto i = 0; i < str.size(); i++)
		glutBitmapCharacter(font, str[i]);
}

void display() {
	// Default Settings
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	string score_str, str;
	if (scene == START) {
		if (blinker) {
			titleTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		}
		else {
			titleTexture2.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		}
	}
	else if (scene == PLAY || scene == PAUSE) {
		if (scene == PAUSE) {

		}

		//Textures
		backgroundTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		frameTexture.drawSquareWithTexture(-3 * PIXEL_SIZE - 20, -6 * PIXEL_SIZE - 30, 3 * PIXEL_SIZE + 20, 6 * PIXEL_SIZE + 30);
		frameTexture.drawSquareWithTexture(4 * PIXEL_SIZE - 10, -20, 5 * PIXEL_SIZE + 10, 2 * PIXEL_SIZE + 20);
		//boardTexture.drawSquareWithTexture(-3 * PIXEL_SIZE, -6 * PIXEL_SIZE, 3 * PIXEL_SIZE, 6 * PIXEL_SIZE);
		keyTexture.drawSquareWithTexture(4 * PIXEL_SIZE, -6 * PIXEL_SIZE, 7 * PIXEL_SIZE, -4 * PIXEL_SIZE);

		//Score Characters
		score_str = to_string(board.getScore());
		str = "      ";
		for (int i = 0; i < score_str.size(); i++) {
			str[str.size() - score_str.size() + i] = score_str[i];
		}
		str = "SCORE:  " + str + " points";
		float color[3] = { 1, 1, 0 };
		displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, color, str, -boundaryX + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE);

		//Key Instructions


		//Board
		//board.draw(WIDTH, HEIGHT);

		//Puyos
		glEnable(GL_LIGHTING);
		glEnable(light.getLightID());
		glEnable(GL_DEPTH_TEST);
		light.draw();
		if (!removed && !removeFinished)
			startPuyo.draw();
		nextPuyo.draw();
		for (int row = 0; row < 12; row++)
			for (int col = 0; col < 6; col++)
				if (arrangedPuyos[row][col].getColor() != NONE)
					arrangedPuyos[row][col].draw();
		for (const Puyo& puyo : dropPuyos)
			puyo.draw();
		glDisable(GL_DEPTH_TEST);
		glDisable(light.getLightID());
		glDisable(GL_LIGHTING);
		if (isIndicatorOn)
			drawIndicator(startPuyo[1].getCenter());
	}
	else if (scene == VICTORY) {

	}
	else if (scene == DEFEAT) {
		cout << "defeat" << endl;
	}

	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	switch (key) {
	case 13: // ENTER
		if (scene == START) {
			scene = PLAY;
		}
		break;
	case 27: // ESC - pause on/off
		if (scene == PLAY) {
			scene = PAUSE;
		}
		else if (scene == PAUSE) {
			scene = PLAY;
		}
		break;
	case 32: // SPACE BAR - drop puyos
		break;
	case 'z': // fps 감소 (연습모드일 때만 가능하도록 추후 변경하기)
	case 'x': // fps 증가
		break;
	default:
		break;
	}
}

void specialKeyDown(int key, int x, int y) {
	if (!keyboardValidator || scene == PAUSE)
		return;

	switch (key) {
	case GLUT_KEY_UP: // CCW rotation
		rotate();
		break;
	case GLUT_KEY_DOWN:
		if (!(collisionDetectionBottom(startPuyo[0]) || collisionDetectionBottom(startPuyo[1]))) {
			startPuyo.manualMove(key, PIXEL_SIZE);
		}
		break;
	case GLUT_KEY_LEFT:
		if (!(collisionDetectionLeft(startPuyo[0]) || collisionDetectionLeft(startPuyo[1])))
			startPuyo.manualMove(key, PIXEL_SIZE);
		break;
	case GLUT_KEY_RIGHT:
		if (!(collisionDetectionRight(startPuyo[0]) || collisionDetectionRight(startPuyo[1])))
			startPuyo.manualMove(key, PIXEL_SIZE);
		break;
	default:
		break;
	}
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);

	double wRatio = (double)w / WIDTH;
	double hRatio = (double)h / HEIGHT;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-boundaryX * wRatio, boundaryX * wRatio, -boundaryY * hRatio, boundaryY * hRatio, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}