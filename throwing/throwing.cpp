#include <stdio.h>
#include <stdlib.h>
#include "point2d.h"

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//Program state flags
bool AIMING;	//Whether the player is aiming the throw
bool ROLLING;	//Whether the ball is currently travelling down lane
bool STRIKING;	//Whether the pins are being struck
bool WAITING;	//Stepped away from the lane, able to choose ball texture
bool HOLDING;	//Whether the user is holding down left click

//Pin state values
bool PINS[10];
int PINS_LEFT;
int PINS_HIT;

//Game state values
int SCORE;
int FRAME_COUNT; //The current round in the game out of 10
int ROLL_COUNT;
int DOUBLE_POINTS;

//Display values
int WINDOW_HEIGHT = 600;
int WINDOW_WIDTH = 800;
float WH_COMP = WINDOW_HEIGHT * 3/4; //The height value for comparisons
float WW_COMP = WINDOW_WIDTH * 3/4; //The width value for comparisons
float FOV = 30;		//Field of view for the camera

//Generic globals
int currentX; 		//Cursor's X position relative to the window
int currentY; 		//Cursor's Y position relative to the window
Point2D startPoint;	//Start point of speed calculation
Point2D endPoint;	//End point of speed calculation
float MAX_SPEED = 100;
float MIN_SPEED = 1;

//Displays the "Power Bar" to the user, also computes speed (will be split functions at a later point)
void drawPowerLine() {

	float d = distance(startPoint, endPoint);
	float speed;
	if (d > WH_COMP) {
		speed = MAX_SPEED;
	} else if (d < (WH_COMP/MAX_SPEED) ) {
		speed = MIN_SPEED;
	} else {
		speed = MAX_SPEED * (d / WH_COMP);
	}

	Point2D p;
	if (startPoint.y >= endPoint.y) {
		p.x = (startPoint.x - endPoint.x) / FOV;	//X value divided by FOV to allow for more precise angles
	} else {
		p.x = (endPoint.x - startPoint.x) / FOV;
	}
	p.y = speed;

	printf("Speed: %f.\n", speed);

	glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glLineWidth(1);
		glVertex3f(0, -5, 0);
		glVertex3f(p.x,-5,-p.y); 
	glEnd();
}



//GL Display Function
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -20);
	//glRotatef(0, -10, 0, 0);

	if (WAITING) {
		//TODO: 
		//drawWaitScene();	
	}
	if (AIMING) {
		//TODO:
		//drawAimScene();
		if (HOLDING) { drawPowerLine(); }
	}
	if (ROLLING) {
		//TODO:
		//rollBall();
	}
	if (STRIKING) {
		//TODO:
		//strikePins();
	}

	glutSwapBuffers();
}

//Regular interactions with keyboard
void keyboard(unsigned char key, int xIn, int yIn) {
	int mod = glutGetModifiers();
	switch (key) {
		case 'q':
		case 27: 	//escape key
			exit(0);
			break;
		case 32:	//Spacebar
			if (!(STRIKING || HOLDING || ROLLING)) {
				AIMING = !AIMING;
				WAITING = !WAITING;
			}
			glutPostRedisplay();
			break;
	}
}

//GL initialization function
void init() {
	glClearColor(1, 1, 1, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, WINDOW_WIDTH/WINDOW_HEIGHT, 0.001f, 1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

}

//Run at the beginning of runtime, to set up the globals properly
void initialSetup() {
	startPoint.x = 0; startPoint.y = 0;
	endPoint.x = 0; endPoint.y = 0;
	AIMING = false;
	HOLDING = false;
	ROLLING = false;
	STRIKING = false;
	WAITING = true;
	FRAME_COUNT = 1;
	ROLL_COUNT = 0;
	for (int i = 0; i < 10; i++) {
		PINS[i] = true;
	}
	PINS_LEFT = 10;
	PINS_HIT = 0;
}

//Capping the FPS to approximatly 60
void FPS(int val) {
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0);
}

//Recognizing mouse interaction
void mouse(int btn, int state, int x, int y) {
	//Converting cursor position to proper coordinates
	int relativeY = WINDOW_HEIGHT - y;
	currentX = x;
	currentY = relativeY;
	printf("Mouse coords: %i,%i\n", x, relativeY);

	if (btn == GLUT_LEFT_BUTTON) {
		printf("Left Mouse Button\n");
		if (AIMING) {

			if (state == GLUT_UP) {		//Left Button released
				HOLDING = false;
				AIMING = false;
				ROLLING = true;
			}

			if (state == GLUT_DOWN) {	//Left Button held
				startPoint.x = x;
				startPoint.y = relativeY;
				endPoint.x = x;
				endPoint.y = relativeY;
				HOLDING = true;
			}
		}
	}

}

//Motion function for mouse
void motion(int x, int y) {
	//Converting cursor position to proper coordinates
	int relativeY = WINDOW_HEIGHT - y;
	currentX = x;
	currentY = relativeY;

	if (AIMING) { //if the current state is that of aiming the ball
		endPoint.x = x;
		endPoint.y = relativeY;
	}
}

//Passive function for mouse
void passive(int x, int y) {
	//Converting cursor position to proper coordinates
	int relativeY = WINDOW_HEIGHT - y;
	currentX = x;
	currentY = relativeY;;
}

void callBackInit() {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutTimerFunc(0, FPS, 0);
}

int main(int argc, char** argv) {
	initialSetup();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
	glutCreateWindow("Bowling Ball Game");
	callBackInit();
	init();
	glutMainLoop();
	return 0;
}