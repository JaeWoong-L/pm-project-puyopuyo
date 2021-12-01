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

#define PI 3.141592
#define SEG 180

#define DROP_VELOCITY Vector3f(0, -2, 0)

#define UP PuyoPuyo::Relation::UP
#define DOWN PuyoPuyo::Relation::DOWN
#define LEFT PuyoPuyo::Relation::LEFT
#define RIGHT PuyoPuyo::Relation::RIGHT

enum Color {
	NONE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	PURPLE
};
// 추가 구현할 부분
//enum Mode {
//	RANKING, // 랭킹 모드 = 스테이지 올라갈수록 속도 빠르게, 뿌요 종류 증가.
//	PRACTICE, // 연습 모드 = fps control로 속도 조절 기능.
//	TOGETHER // 2p 모드 = 둘이서 하는 대결모드
//};

Board board(PIXEL_SIZE);
bool visited[12][6] = { false, };
int dx[] = { 0, 0, -1, 1 };
int dy[] = { 1, -1, 0, 0 };

PuyoPuyo startPuyo, nextPuyo;
vector<Puyo> arrangedPuyos;
Vector3f zeroVector3f;

Light light(boundaryX/2, boundaryX, boundaryX, GL_LIGHT0);
Material red, green, blue, yellow, purple;
Texture backgroundTexture, boardTexture, frameTexture;

clock_t start_t = clock(), end_t;
clock_t blinkStart_t = clock(), blinkEnd_t;
bool isIndicatorOn = true;

// function declarations
void generateRandomColor(Puyo& s, int max);
void drawIndicator(const Vector3f& center);
void remove();
void rotate();
bool collisionDetectionBottom(const Puyo& puyo);
bool collisionDetectionLeft(const Puyo& puyo);
bool collisionDetectionRight(const Puyo& puyo);
void collisionHandling();
void createPuyo();
void initialize();
void idle();
void displayCharacters(void* font, float color[3], string str, float x, float y);
void display();
void keyboardDown(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void reshape(int w, int h);

/***************************************************************
*********************** Main Function **************************
***************************************************************/
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

pair<int, int> getBoardIndex(const Vector3f& pos) {
	const float zeroPos[] = { -3 * PIXEL_SIZE + RADIUS, 6 * PIXEL_SIZE - RADIUS };
	int row = (zeroPos[1] - pos[1]) / PIXEL_SIZE;
	int col = (pos[0] - zeroPos[0]) / PIXEL_SIZE;

	return make_pair(row, col);
}

void remove() {
	queue<Puyo> q;

}

void rotate() {
	Vector3f secondPuyoPos = startPuyo[1].getCenter();

	switch (startPuyo.getRelation()) {
	case UP:
		if (collisionDetectionLeft(startPuyo[1]))
			return;

		if (secondPuyoPos[0] - RADIUS > -boardBoundaryX) {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		else {
			startPuyo[0].setCenter(secondPuyoPos);
			startPuyo[1].setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		startPuyo.setRelation(LEFT);
		break;
	case DOWN:
		if (collisionDetectionRight(startPuyo[1]))
			return;

		if (secondPuyoPos[0] + RADIUS < boardBoundaryX) {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		else {
			startPuyo[0].setCenter(secondPuyoPos);
			startPuyo[1].setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		startPuyo.setRelation(RIGHT);
		break;
	case LEFT:
		if (collisionDetectionBottom(startPuyo[1]))
			return;
		 
		if (secondPuyoPos[1] - RADIUS > -boardBoundaryY) {
			startPuyo[0].setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] - PIXEL_SIZE, 0));
		}
		else {
			startPuyo[0].setCenter(secondPuyoPos);
			startPuyo[1].setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] + PIXEL_SIZE, 0));
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

bool collisionDetectionBottom(const Puyo& puyo) {
	// between puyos
	for (const Puyo& arrangedPuyo : arrangedPuyos)
		if ((puyo && arrangedPuyo) && puyo.getCenter()[0] == arrangedPuyo.getCenter()[0])
			return true;

	// lower boundary
	if (puyo.getCenter()[1] - RADIUS <= -boardBoundaryY)
		return true;

	return false;
}

bool collisionDetectionLeft(const Puyo& puyo) {
	// between puyos
	for (const Puyo& arrangedPuyo : arrangedPuyos)
		if ((puyo && arrangedPuyo) && puyo.getCenter()[1] == arrangedPuyo.getCenter()[1])
			return true;

	// lower boundary
	if (puyo.getCenter()[0] - RADIUS <= -boardBoundaryX)
		return true;

	return false;
}

bool collisionDetectionRight(const Puyo& puyo) {
	// between puyos
	for (const Puyo& arrangedPuyo : arrangedPuyos)
		if ((puyo && arrangedPuyo) && puyo.getCenter()[1] == arrangedPuyo.getCenter()[1])
			return true;

	// lower boundary
	if (puyo.getCenter()[0] + RADIUS >= boardBoundaryX)
		return true;

	return false;
}

/// <summary>
/// 1. 공 겹치는 문제 수정
/// 2. 공 놓여있는 곳 지나면 collisionDetection 판정돼서 좌우로 안움직이는 버그 수정.
/// </summary>
/// <param name="puyo"></param>
/// <returns></returns>

void collisionHandling() {
	if (collisionDetectionBottom(startPuyo[0])) {
		cout << "collision handling 0" << endl;
		startPuyo[0].setVelocity(zeroVector3f);

		if (startPuyo.getRelation() == UP || startPuyo.getRelation() == DOWN)
			startPuyo[1].setVelocity(zeroVector3f);
	}
	if (collisionDetectionBottom(startPuyo[1])) {
		cout << "collision handling 1" << endl;
		startPuyo[1].setVelocity(zeroVector3f);

		if (startPuyo.getRelation() == UP || startPuyo.getRelation() == DOWN)
			startPuyo[0].setVelocity(zeroVector3f);
	}
}

void createPuyo() {
	Vector3f startPuyoPos[2] = { Vector3f(RADIUS, 5 * PIXEL_SIZE + RADIUS, 0),
								 Vector3f(RADIUS, 5 * PIXEL_SIZE - RADIUS, 0) };
	Vector3f nextPuyoPos[2] = { Vector3f(0, 7 * PIXEL_SIZE + RADIUS , 0),
								Vector3f(0, 7 * PIXEL_SIZE - RADIUS , 0) };
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

	//init startPuyo & nextPuyo
	string backgroundImg = "background_christmas_cartoon.jpg";
	backgroundTexture.initializeTexture(backgroundImg.c_str());

	//string boardImg = "background_board.png";
	//boardTexture.initializeTexture(boardImg.c_str());

	string frameImg = "frame_brown.png";
	frameTexture.initializeTexture(frameImg.c_str());

	createPuyo();
}

void idle() {
	//blink indicator

	//move down startPuyo
	collisionHandling();
	if (startPuyo[0].getVelocity()[1] == 0 && startPuyo[1].getVelocity()[1] == 0) {
		arrangedPuyos.push_back(startPuyo[0]);
		arrangedPuyos.push_back(startPuyo[1]);
		createPuyo();
	}
	else if (startPuyo[0].getVelocity()[1] == 0) {
		isIndicatorOn = false;
		
		arrangedPuyos.push_back(startPuyo[0]);
		startPuyo[1].setVelocity(DROP_VELOCITY);
		startPuyo[1].move();
	}
	else if (startPuyo[1].getVelocity()[1] == 0) {
		isIndicatorOn = false;

		arrangedPuyos.push_back(startPuyo[1]);
		startPuyo[0].setVelocity(DROP_VELOCITY);
		startPuyo[0].move();
	}
	else {
		blinkEnd_t = clock();
		if ((blinkEnd_t - blinkStart_t) > CLOCKS_PER_SEC / 6) {
			isIndicatorOn = !isIndicatorOn;

			blinkStart_t = blinkEnd_t;
		}

		end_t = clock();
		if ((end_t - start_t) > CLOCKS_PER_SEC) {
			startPuyo.move();

			start_t = end_t;
		}
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

	//Textures
	backgroundTexture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
	frameTexture.drawSquareWithTexture(-3 * PIXEL_SIZE - 20, -6 * PIXEL_SIZE - 30, 3 * PIXEL_SIZE + 20, 6 * PIXEL_SIZE + 30);
	//boardTexture.drawSquareWithTexture(-3 * PIXEL_SIZE, -6 * PIXEL_SIZE, 3 * PIXEL_SIZE, 6 * PIXEL_SIZE);

	//Score Characters
	string score_str = to_string(board.getScore());
	string str = "      ";
	for (int i = 0; i < score_str.size(); i++) {
		str[str.size() - score_str.size() + i] = score_str[i];
	}
	str = "SCORE:  " + str + " points";
	float color[3] = { 1, 1, 0 };
	displayCharacters(GLUT_BITMAP_TIMES_ROMAN_10, color, str, -boundaryX + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE);

	//Key Instructions
	

	//Board
	//board.draw(WIDTH, HEIGHT);

	//Puyos
	glEnable(GL_LIGHTING);
	glEnable(light.getLightID());
	glEnable(GL_DEPTH_TEST);
	light.draw();
	startPuyo.draw();
	nextPuyo.draw();
	for(const Puyo& arrangedPuyo : arrangedPuyos) {
		if (arrangedPuyo.getColor() != NONE)
			arrangedPuyo.draw();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(light.getLightID());
	glDisable(GL_LIGHTING);
	if (isIndicatorOn)
		drawIndicator(startPuyo[1].getCenter());

	glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC - pause on/off
	case 32: // SPACE BAR - drop puyos
	case 'z': // fps 감소 (연습모드일 때만 가능하도록 추후 변경하기)
	case 'x': // fps 증가
		break;
	default:
		break;
	}
}

void specialKeyDown(int key, int x, int y) {
	// copy startPuyo to temporary puyo p
	PuyoPuyo p = startPuyo;

	switch (key) {
	case GLUT_KEY_UP: // CCW rotation
		rotate();
		break;
	case GLUT_KEY_DOWN:
		//if (!(collisionDetectionBottom(startPuyo[0]) || collisionDetectionBottom(startPuyo[1]))) {
		//	p.manualMove(key, PIXEL_SIZE);
		//	if(!(collisionDetectionBottom(p[0]) || collisionDetectionBottom(p[1])))
		//		startPuyo.manualMove(key, PIXEL_SIZE);
		//}

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