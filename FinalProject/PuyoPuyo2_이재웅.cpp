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
Puyo arrangedPuyos[12][6];

Light light(boundaryX/2, boundaryX, boundaryX, GL_LIGHT0);
Material red, green, blue, yellow, purple;
Texture backgroundTexture, boardTexture, frameTexture;

clock_t start_t = clock(), end_t;
clock_t blinkStart_t = clock(), blinkEnd_t;
bool isIndicatorOn = true;
bool collisionDetected = false;

// function declarations
void generateRandomColor(Puyo& s, int max);
void drawIndicator(const Vector3f& center);
void arrangePuyo(const Puyo& puyo);
void drop(Puyo& puyo);
void remove();
void rotate();
void collisionHandling(Puyo& sph);
void createPuyo();
void initialize();
void idle();
void displayCharacters(void* font, float color[3], string str, float x, float y);
void display();
void keyboardDown(unsigned char key, int x, int y);
void specialKeyDown(int key, int x, int y);
void reshape(int w, int h);

// main function
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

void arrangePuyo(const Puyo& puyo) {
	pair<int, int> boardIndex = getBoardIndex(puyo.getCenter());

	arrangedPuyos[boardIndex.first][boardIndex.second] = puyo;
}

void drop(Puyo& puyo) {
	int row = getBoardIndex(puyo.getCenter()).first;
	int col = getBoardIndex(puyo.getCenter()).second;
	cout << "drop start" << endl;
	cout << "vel: " << puyo.getVelocity()[1] << endl;
	cout << "x: " << puyo.getCenter()[0] << ", y: " << puyo.getCenter()[1] << endl;

	if (puyo.getCenter()[1] < -boundaryY)
		return;

	for (int i = row + 1; i < 12; i++) {
		if (arrangedPuyos[i][col].getColor() == NONE)
			continue;
		while (!(arrangedPuyos[i][col] && puyo)) {
			puyo.move();
		}
		break;
	}

	cout << "drop end" << endl;
}

void remove() {
	queue<Puyo> q;

}

void rotate() {
	Vector3f secondPuyoPos = startPuyo.getSecondPuyo().getCenter();

	switch (startPuyo.getRelation()) {
	case PuyoPuyo::UP:
		if (secondPuyoPos[0] + RADIUS > -boardBoundaryX) {
			startPuyo.getFirstPuyo().setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		else {
			startPuyo.getFirstPuyo().setCenter(secondPuyoPos);
			startPuyo.getSecondPuyo().setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		break;
	case PuyoPuyo::DOWN:
		if (secondPuyoPos[0] + RADIUS < boardBoundaryX) {
			startPuyo.getFirstPuyo().setCenter(Vector3f(secondPuyoPos[0] + PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		else {
			startPuyo.getFirstPuyo().setCenter(secondPuyoPos);
			startPuyo.getSecondPuyo().setCenter(Vector3f(secondPuyoPos[0] - PIXEL_SIZE, secondPuyoPos[1], 0));
		}
		break;
	case PuyoPuyo::LEFT:
		if (secondPuyoPos[1] - RADIUS < -boardBoundaryY) {
			startPuyo.getFirstPuyo().setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] - PIXEL_SIZE, 0));
		}
		else {
			startPuyo.getFirstPuyo().setCenter(secondPuyoPos);
			startPuyo.getSecondPuyo().setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] + PIXEL_SIZE, 0));
		}
		break;
	case PuyoPuyo::RIGHT:
		startPuyo.getFirstPuyo().setCenter(Vector3f(secondPuyoPos[0], secondPuyoPos[1] + PIXEL_SIZE, 0));
		break;
	default:
		break;
	}
}

void collisionHandling(PuyoPuyo& puyos) {
	for (int row = 0; row < 12; row++) {
		for (int col = 0; col < 6; col++) {
			if (arrangedPuyos[row][col] && puyos.getFirstPuyo()) // Collision Detection
				puyos.getFirstPuyo().setVelocity(Vector3f(0, 0, 0));
			if (arrangedPuyos[row][col] && puyos.getSecondPuyo())
				puyos.getSecondPuyo().setVelocity(Vector3f(0, 0, 0));
		}
	}

	if (puyos.getFirstPuyo().getCenter()[1] - RADIUS <= -6 * PIXEL_SIZE)
		puyos.getFirstPuyo().setVelocity(Vector3f(0, 0, 0));
	if (puyos.getSecondPuyo().getCenter()[1] - RADIUS <= -6 * PIXEL_SIZE)
		puyos.getSecondPuyo().setVelocity(Vector3f(0, 0, 0));
}

void createPuyo() {
	Vector3f startPuyoPos[2] = { Vector3f(-RADIUS, 6 * PIXEL_SIZE - RADIUS, 0),
								 Vector3f(RADIUS, 6 * PIXEL_SIZE - RADIUS, 0) };
	Vector3f nextPuyoPos[2] = { Vector3f(0, 7 * PIXEL_SIZE + RADIUS , 0),
								Vector3f(0, 7 * PIXEL_SIZE - RADIUS , 0) };
	Vector3f initialVelocity(0, -PIXEL_SIZE / 2, 0);

	if (startPuyo.getFirstPuyo().getColor() == NONE) {
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
		startPuyo.getFirstPuyo().setCenter(startPuyoPos[0]);
		startPuyo.getSecondPuyo().setCenter(startPuyoPos[1]);

		generateRandomColor(startPuyo.getFirstPuyo(), 4);
		generateRandomColor(startPuyo.getSecondPuyo(), 4);
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
	blinkEnd_t = clock();
	if ((blinkEnd_t - blinkStart_t) > CLOCKS_PER_SEC / 6) {
		isIndicatorOn = !isIndicatorOn;

		blinkStart_t = blinkEnd_t;
	}

	//move down startPuyo
	end_t = clock();
	if ((end_t - start_t) > CLOCKS_PER_SEC) {
		collisionHandling(startPuyo);
		if(collisionDetected)

		startPuyo[0].move();
		startPuyo[1].move();

		start_t = end_t;
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
	for (int row = 0; row < 12; row++) {
		for (int col = 0; col < 6; col++) {
			if (arrangedPuyos[row][col].getColor() != NONE)
				arrangedPuyos[row][col].draw();
		}
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(light.getLightID());
	glDisable(GL_LIGHTING);
	if (isIndicatorOn)
		drawIndicator(startPuyo.getSecondPuyo().getCenter());

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
	bool movable = true;

	switch (key) {
	case GLUT_KEY_UP: // CCW rotation
		rotate();
		break;
	case GLUT_KEY_DOWN:
		for (const Puyo& puyo : startPuyo) {
			if (puyo.getCenter()[1] - RADIUS <= -6 * PIXEL_SIZE) {
				movable = false;
				break;
			}
		}

		if (movable)
			for (Puyo& puyo : startPuyo)
				puyo.manualMove(key, PIXEL_SIZE);
		break;
	case GLUT_KEY_LEFT: 
		for (const Puyo& puyo : startPuyo) {
			if (puyo.getCenter()[0] - RADIUS <= -3 * PIXEL_SIZE) {
				movable = false;
				break;
			}
		}

		if (movable)
			for (Puyo& puyo : startPuyo)
				puyo.manualMove(key, PIXEL_SIZE);
		break;
	case GLUT_KEY_RIGHT:
		for (const Puyo& puyo : startPuyo) {
			if (puyo.getCenter()[0] + RADIUS >= 3 * PIXEL_SIZE) {
				movable = false;
				break;
			}
		}

		if (movable)
			for (Puyo& puyo : startPuyo)
				puyo.manualMove(key, PIXEL_SIZE);
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