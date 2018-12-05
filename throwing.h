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

void release(){

}

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
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(0, 0, 0);
		glLineWidth(1);
		glVertex3f(0, -5, 0);
		glVertex3f(p.x,-5,-p.y);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
