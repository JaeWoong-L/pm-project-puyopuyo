#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "Windows.h"
#include "conio.h"
#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")

#include "Light.h"
#include "Board.h"
#include "Puyo.h"
#include "PuyoPuyo.h"
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

enum Scene {
	START,
	PLAY,
	PAUSE,
	VICTORY,
	DEFEAT,
	RANK
};

Scene scene = START;
Board board(PIXEL_SIZE);
PuyoPuyo startPuyo, nextPuyo;
vector<Puyo> dropPuyos;
Puyo emptyPuyo;
Vector3f dropVelocity(0, -2, 0);
Vector3f zeroVector3f;
vector<pair<int, bool> > ranking; // first: score, second: isNew
int fadeOutCount = 200;
int finalScore = 0;

Light light(boundaryX/2, boundaryX, boundaryX, GL_LIGHT0);
Material red, green, blue, yellow, purple, sky;
Texture titleTexture, titleTexture2, backgroundTexture, frameTexture, victoryTexture, defeatTexture;

clock_t start_t = clock(), end_t;
clock_t blinkStart_t = clock(), blinkEnd_t;
bool isIndicatorOn = true;
bool isTextureBlinkerOn = true;
bool keyboardValidator = true;
bool removed = false;
bool removeFinished = false;
bool drop = false;

// function declarations
void generateRandomColor(Puyo& s, int stage);
void drawIndicator(const Vector3f& center);
bool compareRank(pair<int, bool>& lhs, pair<int, bool>& rhs);
bool collisionDetectionBottom(const Puyo& puyo);
bool collisionDetectionLeft(const Puyo& puyo);
bool collisionDetectionRight(const Puyo& puyo);
void collisionHandling();
void rotate();
void setDropPuyos();
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
	if(scene == START || scene == PLAY)
		glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}

// function definitions
void generateRandomColor(Puyo &s, int stage) {
	Puyo::Color color = static_cast<Puyo::Color>(rand() % (stage + 3) + 1);
	switch (color) {
	case Puyo::Color::RED:
		s.setMTL(red);
		s.setColor(Puyo::Color::RED);
		break;
	case Puyo::Color::GREEN:
		s.setMTL(green);
		s.setColor(Puyo::Color::GREEN);
		break;
	case Puyo::Color::BLUE:
		s.setMTL(blue);
		s.setColor(Puyo::Color::BLUE);
		break;
	case Puyo::Color::YELLOW:
		s.setMTL(yellow);
		s.setColor(Puyo::Color::YELLOW);
		break;
	case Puyo::Color::PURPLE:
		s.setMTL(purple);
		s.setColor(Puyo::Color::PURPLE);
		break;
	case Puyo::Color::SKY:
		s.setMTL(sky);
		s.setColor(Puyo::Color::SKY);
		break;
	default:
		break;
	}
}

void drawIndicator(const Vector3f& center) {
	/* Implement: draw all circles */
	glColor3f(0.7f, 0.7f, 0.7f);
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= SEG; i++) {
		float angle = i * (2 * PI / SEG);
		float dx = (RADIUS + 0.5) * cos(angle);
		float dy = (RADIUS + 0.5) * sin(angle);
		glVertex3f(center[0] + dx, center[1] + dy, center[2] + 1);
	}
	glEnd();
}

bool compareRank(pair<int, bool>& lhs, pair<int, bool>& rhs) {
	return lhs.first > rhs.first;
}

bool collisionDetectionBottom(const Puyo& puyo) {
	// between puyos
	for(int row = 0; row < 12; row++)
		for(int col = 0; col < 6; col++)
			if (board.getPuyo(row, col).getColor() != Puyo::Color::NONE
				&& (puyo && board.getPuyo(row, col))
				&& puyo.getCenter()[0] == board.getPuyo(row, col).getCenter()[0]
				&& puyo.getCenter()[1] > board.getPuyo(row, col).getCenter()[1])
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
			if (board.getPuyo(row, col).getColor() != Puyo::Color::NONE
				&& (puyo && board.getPuyo(row, col))
				&& puyo.getCenter()[0] > board.getPuyo(row, col).getCenter()[0])
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
			if (board.getPuyo(row, col).getColor() != Puyo::Color::NONE
				&& (puyo && board.getPuyo(row, col))
				&& puyo.getCenter()[0] < board.getPuyo(row, col).getCenter()[0])
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

void setDropPuyos() {
	for (int row = 11; row >= 0; row--) {
		for (int col = 0; col < 6; col++) {
			if (board.getPuyo(row, col).getColor() != Puyo::Color::NONE) {
				board.getPuyo(row, col).setVelocity(dropVelocity);
				dropPuyos.push_back(board.getPuyo(row, col));
				board.getPuyo(row, col) = emptyPuyo;
			}
			else
				board.getPuyo(row, col).setVelocity(zeroVector3f);
		}
	}
}

void createPuyo() {
	Vector3f startPuyoPos[2] = { Vector3f(-RADIUS, 5 * PIXEL_SIZE + RADIUS, 0),
								 Vector3f(-RADIUS, 5 * PIXEL_SIZE - RADIUS, 0) };
	Vector3f nextPuyoPos[2] = { Vector3f(4.5 * PIXEL_SIZE, 1.5 * PIXEL_SIZE, 0),
								Vector3f(4.5 * PIXEL_SIZE, 0.5 * PIXEL_SIZE, 0) };
	Vector3f initialVelocity(0, -PIXEL_SIZE, 0);

	if (startPuyo[0].getColor() == Puyo::Color::NONE) {
		Puyo sph1(RADIUS, 20, 20), sph2(RADIUS, 20, 20);

		//init startPuyo
		sph1.setCenter(startPuyoPos[0]);
		sph1.setVelocity(initialVelocity);
		generateRandomColor(sph1, board.getStage());

		sph2.setCenter(startPuyoPos[1]);
		sph2.setVelocity(initialVelocity);
		generateRandomColor(sph2, board.getStage());

		startPuyo.setPuyos(sph1, sph2);

		//init nextPuyo
		sph1.setCenter(nextPuyoPos[0]);
		generateRandomColor(sph1, board.getStage());

		sph2.setCenter(nextPuyoPos[1]);
		generateRandomColor(sph2, board.getStage());

		nextPuyo.setPuyos(sph1, sph2);
	}
	else {
		startPuyo = nextPuyo;
		startPuyo[0].setCenter(startPuyoPos[0]);
		startPuyo[1].setCenter(startPuyoPos[1]);

		generateRandomColor(nextPuyo[0], board.getStage());
		generateRandomColor(nextPuyo[1], board.getStage());
	}
}

void initialize() {
	//read ranking.txt
	ifstream ifs("ranking.txt");
	long long val;
	while (ifs >> val, !ifs.eof()) {
		ranking.push_back(make_pair(val, false));
	}
	ifs.close();
	//init materials
	red.setAmbient(Vector4f(1, 0, 0, 0));
	green.setAmbient(Vector4f(0, 1, 0, 0));
	blue.setAmbient(Vector4f(0, 0, 1, 0));
	yellow.setAmbient(Vector4f(1, 1, 0, 0));
	purple.setAmbient(Vector4f(1, 0, 1, 0));
	sky.setAmbient(Vector4f(0, 1, 1, 0));

	//init textures
	string titleImg = "image/title.png";
	titleTexture.initializeTexture(titleImg.c_str());
	
	string titleImg2 = "image/title2.png";
	titleTexture2.initializeTexture(titleImg2.c_str());

	string backgroundImg = "image/background_christmas_cartoon.jpg";
	backgroundTexture.initializeTexture(backgroundImg.c_str());

	string victoryImg = "image/victory.png";
	victoryTexture.initializeTexture(victoryImg.c_str());

	string defeatImg = "image/defeat.png";
	defeatTexture.initializeTexture(defeatImg.c_str());

	string frameImg = "image/frame_brown.png";
	frameTexture.initializeTexture(frameImg.c_str());

	createPuyo();
}

void idle() {
	if (scene == START) {
		end_t = clock();
		if (end_t - start_t > CLOCKS_PER_SEC / 2) {
			isTextureBlinkerOn = !isTextureBlinkerOn;
			start_t = end_t;
		}
	}
	else if (scene == PLAY) {
		if (board.getPuyo(0, 2).getColor() != Puyo::Color::NONE) {
			scene = DEFEAT;
			if (board.getStage() == 3) {
				// score ranking.txt에 저장하기
				finalScore = board.getScore();
				if (finalScore > ranking.back().first) {
					scene = RANK;
					ranking.pop_back();
					ranking.push_back(make_pair(finalScore, true));
					sort(ranking.begin(), ranking.end(), compareRank);
				}

				ofstream ofs;
				ofs.open("ranking.txt");
				for (auto data : ranking) {
					ofs << data.first << endl;
				}
				ofs.close();
			}
			drop = false;
			removed = false;
			removeFinished = false;
			keyboardValidator = true;
		}
		else if (board.getScore() >= 1000 && board.getStage() < 3) {
			// stage 3 = infinity mode
			scene = VICTORY;
			drop = false;
			removed = false;
			removeFinished = false;
			keyboardValidator = true;
		}
		else if (removed) {
			if (drop) {
				if (dropPuyos.empty())
					setDropPuyos();

				int moveCount = 0;
				for (Puyo& puyo : dropPuyos) {
					if (!collisionDetectionBottom(puyo)) {
						puyo.move();
						moveCount++;
					}
					else {
						puyo.setVelocity(zeroVector3f);
					}
				}

				if (moveCount == 0) {
					for (const Puyo& puyo : dropPuyos)
						board.setPuyo(puyo);
					dropPuyos.clear();

					fadeOutCount = 200;
					removed = false;
					drop = false;
					removeFinished = true;
				}
			}
			else if (fadeOutCount <= 0) { // remove finished
				for (int row = 0; row < 12; row++) {
					for (int col = 0; col < 6; col++) {
						if (board.getPuyo(row, col).getRemoved()) {
							board.getPuyo(row, col).setColor(Puyo::Color::NONE);
							board.getPuyo(row, col).setRemoved(false);
						}
					}
				}

				drop = true;
			}
			else {
				isIndicatorOn = false;
				keyboardValidator = false;

				for (int row = 0; row < 12; row++) {
					for (int col = 0; col < 6; col++) {
						if (board.getPuyo(row, col).getRemoved()) {
							board.getPuyo(row, col).fadeOut(fadeOutCount);
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
					board.setPuyo(startPuyo[0]);
					board.setPuyo(startPuyo[1]);
					keyboardValidator = true;
					removeFinished = true;
				}

				removed = board.hasPuyoToRemove();
				if (!removed && removeFinished) {
					createPuyo();
					board.setCombo(0);
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
				if ((end_t - start_t) > CLOCKS_PER_SEC - (board.getStage() - 1) * 200 ) {
					startPuyo.move();

					start_t = end_t;
				}
			}
		}
	}

	glutPostRedisplay();
}

void displayCharacters(void* font, float color[3], string str, float x, float y) {
	glColor3f(color[0], color[1], color[2]);
	glRasterPos2f(x, y);
	for (unsigned int i = 0; i < str.size(); i++)
		glutBitmapCharacter(font, str[i]);
}

void display() {
	// Default Settings
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (scene == START) {
		if (isTextureBlinkerOn) {
			titleTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		}
		else {
			titleTexture2.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		}
	}
	else if (scene == PLAY || scene == PAUSE) {
		//Textures
		backgroundTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
		frameTexture.drawSquareWithTexture(-3 * PIXEL_SIZE - 20, -6 * PIXEL_SIZE - 30, 3 * PIXEL_SIZE + 20, 6 * PIXEL_SIZE + 30);
		frameTexture.drawSquareWithTexture(4 * PIXEL_SIZE - 10, -20, 5 * PIXEL_SIZE + 10, 2 * PIXEL_SIZE + 20);

		//Score Characters
		string stage_str = "STAGE " + to_string(board.getStage());
		if (board.getStage() == 3) {
			stage_str += " - INFINITY MODE";
		}
		string score_str = to_string(board.getScore());
		string str = "      ";
		for (unsigned int i = 0; i < score_str.size(); i++) {
			str[str.size() - score_str.size() + i] = score_str[i];
		}
		score_str = "SCORE:  " + str + " points";
		float color[3] = { 0, 0, 0 };
		displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, color, stage_str, -boundaryX + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE);
		displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, color, score_str, -boundaryX + PIXEL_SIZE / 2, boundaryY - 2 * PIXEL_SIZE);

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
				if (board.getPuyo(row, col).getColor() != Puyo::Color::NONE)
					board.getPuyo(row, col).draw();
		for (const Puyo& puyo : dropPuyos)
			puyo.draw();
		glDisable(GL_DEPTH_TEST);
		glDisable(light.getLightID());
		glDisable(GL_LIGHTING);
		if (isIndicatorOn)
			drawIndicator(startPuyo[1].getCenter());
	}
	else if (scene == VICTORY) {
		victoryTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
	}
	else if (scene == DEFEAT) {
		defeatTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
	}
	else if (scene == RANK) {
		float black[3] = { 1, 1, 1 };
		float red[3] = { 1, 0, 0 };
		string rankingTitle = "RANKING";
		displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, black, rankingTitle, -1.5 * PIXEL_SIZE, boundaryY - 2 *PIXEL_SIZE);
		string ranker;
		for (int i = 0; i < ranking.size(); i++) {
			ranker = to_string(i + 1) + ". " + to_string(ranking[i].first) + " points";
			if (ranking[i].second == true)
				displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, red, ranker, -2 * PIXEL_SIZE, boundaryY - 3 * PIXEL_SIZE - (i + 1) * PIXEL_SIZE);
			else
				displayCharacters(GLUT_BITMAP_TIMES_ROMAN_24, black, ranker, -2 * PIXEL_SIZE, boundaryY - 3 * PIXEL_SIZE - (i+1) * PIXEL_SIZE);
		}
	}

	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	if (!keyboardValidator)
		return;

	if (scene == VICTORY) {
		board.setStage(board.getStage() + 1);
		board.clear();
		startPuyo.clear();
		nextPuyo.clear();
		createPuyo();

		scene = PLAY;
		return;
	}
	else if (scene == DEFEAT) {
		board.setStage(1);
		board.clear();
		startPuyo.clear();
		nextPuyo.clear();
		createPuyo();

		scene = PLAY;
		return;
	}

	switch (key) {
	case 13: // ENTER
		if (scene == START) {
			PlaySound(TEXT("./sound/title_enter.wav"), NULL, SND_FILENAME | SND_ASYNC);
			scene = PLAY;
		}
		else if (scene == RANK) {
			scene = START;
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
		if (scene == PLAY) {
			while (!(collisionDetectionBottom(startPuyo[0]) || collisionDetectionBottom(startPuyo[1]))) {
				startPuyo.move();
			}
			if (collisionDetectionBottom(startPuyo[0])) {
				startPuyo[0].setVelocity(zeroVector3f);
				board.setPuyo(startPuyo[0]);
			}
			if (collisionDetectionBottom(startPuyo[1])) {
				startPuyo[1].setVelocity(zeroVector3f);
				board.setPuyo(startPuyo[1]);
			}
		}
		break;
	default:
		break;
	}
}

void specialKeyDown(int key, int x, int y) {
	if (!keyboardValidator || scene != PLAY)
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