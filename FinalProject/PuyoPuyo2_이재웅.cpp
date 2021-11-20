#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>

#include "Light.h"
#include "Sphere.h"
#include "Board.h"
#include "Texture.h"

using namespace std;

// 8:15 비율 8:5  1200:750 120 960 
#define WIDTH 960
#define HEIGHT 600
#define PIXEL_SIZE 40 // 한 칸 : 40
#define RADIUS PIXEL_SIZE/2 // 반지름: 20

#define boundaryX WIDTH/2
#define boundaryY HEIGHT/2

#define PI 3.141592
#define SEG 360

enum Color {
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
vector<Sphere> startPuyo;
vector<Sphere> nextPuyo;
vector<Sphere> arrangedPuyos;

Light light(boundaryX/2, boundaryX, boundaryX, GL_LIGHT0);
Material red, green, blue, yellow, purple;
Texture texture;
Texture texture2;

clock_t start_t = clock(), end_t;
clock_t blinkStart_t = clock(), blinkEnd_t;
bool isIndicatorOn = true;

void generateRandomColor(Sphere &s, int max) {
	int color = rand() % max;
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

void drawCircle(const float center[3]) {
	/* Implement: draw all circles */
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(4.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= SEG; i++) {
		float angle = i * (2 * PI / SEG);
		float dx = (RADIUS) * cos(angle);
		float dy = (RADIUS) * sin(angle);
		glVertex3f(center[0] + dx, center[1] + dy, center[2] + 1);
	}
	glEnd();
}

void createPuyos() {
	float startPuyoPos[2][2] = { {-boundaryX + 3 * PIXEL_SIZE + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE - PIXEL_SIZE / 2},
								 {-boundaryX + 3 * PIXEL_SIZE + PIXEL_SIZE / 2, boundaryY - 2 * PIXEL_SIZE - PIXEL_SIZE / 2} };
	float nextPuyoPos[2][2] = { {-boundaryX + 3 * PIXEL_SIZE + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE / 2},
								 {-boundaryX + 4 * PIXEL_SIZE + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE / 2} };

	if (startPuyo.empty() && nextPuyo.empty()) { //initialize
		Sphere S1(RADIUS, 20, 20), S2(RADIUS, 20, 20);

		//init startPuyo
		S1.setCenter(startPuyoPos[0][0], startPuyoPos[0][1], 0);
		S1.setVelocity(0, -PIXEL_SIZE / 2, 0);
		generateRandomColor(S1, 4);

		S2.setCenter(startPuyoPos[1][0], startPuyoPos[1][1], 0);
		S2.setVelocity(0, -PIXEL_SIZE / 2, 0);
		generateRandomColor(S2, 4);

		startPuyo.push_back(S1);
		startPuyo.push_back(S2);

		//init nextPuyo
		S1.setCenter(nextPuyoPos[0][0], nextPuyoPos[0][1], 0);
		generateRandomColor(S1, 4);
		
		S2.setCenter(nextPuyoPos[1][0], nextPuyoPos[1][1], 0);
		generateRandomColor(S2, 4);

		nextPuyo.push_back(S1);
		nextPuyo.push_back(S2);
	}
	else { //change
		//startPuyo
		for (int i = 0; i < 2; i++) {
			startPuyo[i] = nextPuyo[i];
			startPuyo[i].setCenter(startPuyoPos[i][0], startPuyoPos[i][1], 0);
		}

		//nextPuyo
		for (int i = 0; i < 2; i++)
			generateRandomColor(nextPuyo[i], 4);
	}
}

// 알고리즘 구현부분
void remove() {

}

void displayCharacters(void* font, float color[3], string str, float x, float y) {
	glColor3f(color[0], color[1], color[2]);
	glRasterPos2f(x, y);
	for (auto i = 0; i < str.size(); i++)
		glutBitmapCharacter(font, str[i]);
}

void initialize() {
	//init materials
	red.setAmbient(1, 0, 0, 0);
	green.setAmbient(0, 1, 0, 0);
	blue.setAmbient(0, 0, 1, 0);
	yellow.setAmbient(1, 1, 0, 0);
	purple.setAmbient(1, 0, 1, 0);

	//init startPuyo & nextPuyo
	string filename = "background_christmas_dark.png";
	texture.initializeTexture(filename.c_str());

	string filename2 = "background_christmas_vertical.jpg";
	texture2.initializeTexture(filename2.c_str());

	createPuyos();
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
		for (int i = 0; i < 2; i++) {
			startPuyo[i].setCenter(startPuyo[i].getCenter()[0] + startPuyo[i].getVelocity()[0],
				startPuyo[i].getCenter()[1] + startPuyo[i].getVelocity()[1],
				startPuyo[i].getCenter()[2] + startPuyo[i].getVelocity()[2]);
		}
		createPuyos();

		start_t = end_t;
	}
	

	glutPostRedisplay();
}

void display() {
	// Default Settings
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-boundaryX, boundaryX, -boundaryY, boundaryY, -100.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();*/

	//Background
	texture.drawSquareWithTexture(-boundaryX, -boundaryY, boundaryX, boundaryY);
	texture2.drawSquareWithTexture(-3 * PIXEL_SIZE, -6 * PIXEL_SIZE, 3 * PIXEL_SIZE, 6 * PIXEL_SIZE);

	//Score Characters
	string score_str = to_string(board.getScore());
	string str = "      ";
	for (auto i = 0; i < score_str.size(); i++) {
		str[str.size() - score_str.size() + i] = score_str[i];
	}
	str = "SCORE:  " + str + " points";
	float color[3] = { 1, 1, 0 };
	displayCharacters(GLUT_BITMAP_TIMES_ROMAN_10, color, str, -boundaryX + PIXEL_SIZE / 2, boundaryY - PIXEL_SIZE);

	//Key Instructions
	

	//Board
	//board.draw(WIDTH, HEIGHT);

	//Puyos
	if(isIndicatorOn)
		drawCircle(startPuyo[1].getCenter());
	glEnable(GL_LIGHTING);
	glEnable(light.getLightID());
	glEnable(GL_DEPTH_TEST);
	light.draw();
	for (int i = 0; i < 2; i++) {
		startPuyo[i].draw();
		nextPuyo[i].draw();
	}
	glDisable(GL_DEPTH_TEST);
	glDisable(light.getLightID());
	glDisable(GL_LIGHTING);

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

void keyboardUp(unsigned char key, int x, int y) {

}

void specialKeyDown(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP: // CCW rotation

		break;
	case GLUT_KEY_DOWN: // move down(one pixel)
		break;
	case GLUT_KEY_LEFT: // move left(one pixel)
		break;
	case GLUT_KEY_RIGHT: // move right(one pixel)
		break;
	default:
		break;
	}
}

//void specialKeyUp(int key, int x, int y) {
//
//}

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
	//glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialKeyDown);
	//glutSpecialUpFunc(specialKeyUp);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}
