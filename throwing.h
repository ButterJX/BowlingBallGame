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

//Generic globals
int currentX; 		//Cursor's X position relative to the window
int currentY; 		//Cursor's Y position relative to the window
int startX;
int startY;
Point2D startPoint;	//Start point of speed calculation
Point2D endPoint;	//End point of speed calculation
float MAX_SPEED = 100;
float MIN_SPEED = 1;

//Display values
int WINDOW_HEIGHT = 600;
int WINDOW_WIDTH = 800;
float WH_COMP = WINDOW_HEIGHT * 3/4; //The height value for comparisons
float WW_COMP = WINDOW_WIDTH * 3/4; //The width value for comparisons
float FOV = 30;		//Field of view for the camera

void setPos(int x, int y){
	currentX = x;
	currentY = y;
}

void start(int x, int y){
	startPoint.x = (float)x;
	startPoint.y = (float)y;
	startX = x;
	startY = y;
}

void release(int x, int y, float* fl){
	float r;
	endPoint.x = (float)x;
	endPoint.y = (float)y;
	r = sqrt(pow(endPoint.x - startPoint.x, 2.0) + pow(endPoint.y - startPoint.y, 2.0));
	if(r == 0){
		r = 1.0;
	}
	float d = distance(startPoint, endPoint);
	if (d > WH_COMP) {
		fl[0] = MAX_SPEED;
	} else if (d < (WH_COMP/MAX_SPEED) ) {
		fl[0] = MIN_SPEED;
	} else {
		fl[0] = MAX_SPEED * (d / WH_COMP);
	}
	fl[1] = (endPoint.x - startPoint.x) / r;
	fl[2] = (endPoint.y - startPoint.y) / r;
}

void drawPowerLine() {
	Point2D p;
	if (startPoint.y >= endPoint.y) {
		p.x = (startPoint.x - endPoint.x) / FOV;	//X value divided by FOV to allow for more precise angles
	} else {
		p.x = (endPoint.x - startPoint.x) / FOV;
	}
	p.y = (float)currentY;

	printf("Speed: %f.\n", p.y);

	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT, 0.0, 1.0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glLineWidth(1);
		glVertex3f((startX / 2) + (WINDOW_WIDTH / 4), (startY / 2) + (WINDOW_HEIGHT / 4), 0.0);
		glVertex3f((currentX / 2) + (WINDOW_WIDTH / 4), (currentY / 2) + (WINDOW_HEIGHT / 4), 0.0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	printf("%i, %i\n", currentX, currentY);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}
