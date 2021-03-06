#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <ctime>
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

using namespace std;

//Program state flags
bool AIMING;    //Whether the player is aiming the throw
bool ROLLING;   //Whether the ball is currently travelling down lane
bool STRIKING;  //Whether the pins are being struck
bool WAITING;   //Stepped away from the lane, able to choose ball texture
bool HOLDING;   //Whether the user is holding down left click

//pin state values
bool PINS[10];
int PINS_LEFT;
int CURRENT_PINS_HIT;

//game state values
int SCORE;
int FRAME_COUNT;
int ROLL_COUNT;
int DOUBLE_POINTS;

//ball values
float x_vel;
float y_vel;
float x_pos;
float y_pos;

//Display values
int WINDOW_HEIGHT = 600;
int WINDOW_WIDTH = 800;
float WH_COMP = WINDOW_HEIGHT * 3/4; //The height value for comparisons
float WW_COMP = WINDOW_WIDTH * 3/4; //The width value for comparisons
float FOV = 30;     //Field of view for the camera

//Globals
float throwCamPos[] = {0.0f, 2.0f, 1.0f};	//where the camera is when throwing
float pinCamPos[] = {0.0f, -8.5f, 1.5f};    //where the camera is when hitting pins
float ballCamPos[] = {0.8f, 1.8f, 0.8f};    //follow postion wrt ball
int currentX;       //Cursor's X position relative to the window
int currentY;       //Cursor's Y position relative to the window
Point2D startPoint; //Start point of speed calculation
Point2D endPoint;   //End point of speed calculation
float MAX_SPEED = 50;
float MIN_SPEED = 1;
float speed;

//Updates speed given the current start and end points
void updateSpeed() {
    float d = distance(startPoint, endPoint);
    if (d > WH_COMP) {
        speed = MAX_SPEED;
    } else if (d < (WH_COMP/10) ) {
        speed = MIN_SPEED;
    } else {
        speed = MAX_SPEED * (d / WH_COMP);
    }
    //printf("d: %f, wh_comp: %f\n", d, WH_COMP);

}

//Displays the "Power Bar" to the user, also computes speed (will be split functions at a later point)
void drawPowerLine() {
    updateSpeed();

    Point2D p;
    if (startPoint.y >= endPoint.y) {
        p.x = (startPoint.x - endPoint.x) / FOV;    //X value divided by FOV to allow for more precise angles
    } else {
        p.x = (endPoint.x - startPoint.x) / FOV;
    }
    p.y = speed/MAX_SPEED; //Capping at 1

    x_vel = -p.x/(2*FOV); //X is inverted in this angle
    y_vel = -p.y;

    printf("p.x: %f, p.y: %f, speed: %f.\n", p.x, p.y, speed);

    glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glLineWidth(1);
        glVertex3f(0, -0.5, 1.5);
        glVertex3f(x_vel,-0.5-y_vel, 1.5); 
    glEnd();
}

void throw_ball(){
    AIMING = false;
    HOLDING =  false;
    ROLLING = true;
    //y_vel = -0.04;
}

void strike_pins(){
    if (!STRIKING){
        ROLLING = false;
        STRIKING = true;
        PINS_LEFT = 0;
        int random = rand();
        //randomly hits or misses pins. in reality, we will want to iterate through each pin to check whether it has changed position or not
        for (int i = 0; i < 10; i++){
           if (PINS[i]){
               random = rand();
               PINS[i] = random%2;
               if (!PINS[i]){
                   CURRENT_PINS_HIT++;
                }
            }
            if (PINS[i]){
                PINS_LEFT++;
            }
        }
    }
}

void next_turn(){
    STRIKING = false;
    AIMING = true;
    
    //for determining the next frame and roll
    if (FRAME_COUNT < 10){
        if (ROLL_COUNT == 0){
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins! " + to_string(PINS_LEFT) + " remain this frame.\n";
            ROLL_COUNT++;
        }
        else{
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins and " + to_string(10 - PINS_LEFT) + " in total this frame.\n";
            ROLL_COUNT = 0;
            for (int i = 0; i < 10; i++){ PINS[i] = true; }
            FRAME_COUNT++;
        }
    }
    else {
        if (ROLL_COUNT < 2){
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins! " + to_string(PINS_LEFT) + " remain this frame.\n";
            ROLL_COUNT++;
        }
        else{
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins and " + to_string(10 - PINS_LEFT) + " in total this frame.\n";
            ROLL_COUNT = 0;
            for (int i = 0; i < 10; i++){ PINS[i] = true; }
            FRAME_COUNT++;
        }
    }
    
    //for determining the score
    //when a strike is bowled, the next two rolls' scores are added to that frame
    //when a spare is bowled, the next roll's scores are added to that frame
    //effectively, after a strike is bowled, the next two rolls are worth double,
    //and after a spare, the next roll is worth double
    
    if (DOUBLE_POINTS){
        SCORE += 2*CURRENT_PINS_HIT;
        DOUBLE_POINTS--;
    }

    else {
        SCORE += CURRENT_PINS_HIT;
    }

    CURRENT_PINS_HIT = 0;
    
    if (PINS_LEFT == 0){
        if (ROLL_COUNT == 1){ 
            cout << "STRIKE!!\n";
            DOUBLE_POINTS = 2;
            ROLL_COUNT = 0;
            FRAME_COUNT++;
            for (int i = 0; i < 10; i++){ PINS[i] = true; }
        }
        else {
            DOUBLE_POINTS = 1;
            cout << "SPARE!\n";
            if (FRAME_COUNT == 10 && ROLL_COUNT == 2){
                for (int i = 0; i < 10; i++){ PINS[i] = true; }
            }
        }
    }
    if (FRAME_COUNT == 11) {
        cout << "GAME OVER! Your final score was " + to_string(SCORE) + ".\n";
        cout << "Thank you for playing Ultra Optimized 60fps Bowling Simulator!\n\n";
        glutLeaveMainLoop();
    }
    else {
        cout << "\nRoll: " + to_string(ROLL_COUNT+1) + " Frame: " + to_string(FRAME_COUNT) + " Score: " + to_string(SCORE);
        cout << "\n";
    }
}

//OpenGL functions
void keyboard(unsigned char key, int xIn, int yIn)
{
	int mod = glutGetModifiers();
	switch (key)
	{
		case 't':
            if (AIMING) throw_ball();
            break;
        case 'q':
        case 27:    //escape key
            exit(0);
            break;
        case 32:    //Spacebar
            if (!(STRIKING || HOLDING || ROLLING)) {
                AIMING = !AIMING;
                WAITING = !WAITING;
            }
            glutPostRedisplay();
            break;
	}
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45, 1, 1, 100);
    gluPerspective(FOV, WINDOW_WIDTH/WINDOW_HEIGHT, 0.001f, 1000);
}

/* display function - GLUT display callback function
 * clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    if (WAITING)
        gluLookAt(throwCamPos[0] + x_pos, throwCamPos[1] + y_pos + 2, throwCamPos[2], 0.0, -9.5f, 0.0, 0,0,1);
	else if (AIMING) {
        gluLookAt(throwCamPos[0] + x_pos, throwCamPos[1] + y_pos, throwCamPos[2], 0.0, -9.5f, 0.0, 0,0,1);
        if (HOLDING) {drawPowerLine();}
    }
    else if ((ROLLING && y_pos < -7.5) || STRIKING)
        gluLookAt(pinCamPos[0], pinCamPos[1], pinCamPos[2], 0.0, -9.5f, 0.0, 0,0,1);
    else if (ROLLING)
        gluLookAt(ballCamPos[0] + x_pos, ballCamPos[1] + y_pos, ballCamPos[2], x_pos, y_pos, 0.0, 0,0,1);

    glPushMatrix();
        //lane
        glTranslatef(0.0, -5.0, 0.0);
        glScalef(2.0, 20.0, 0.1);
        glColor3f(0.5f, 0.35f, 0.05f);
        glutSolidCube(0.5);
        glPushMatrix();
            //gutters
            glScalef(0.2, 1.0, 1.0);
            glTranslatef(-1.6, 0.0, -0.1);
            glColor3f(0.25f, 0.25f, 0.25f);
            glutSolidCube(0.5);
            glTranslatef(3.2, 0.0, 0.0);
            glutSolidCube(0.5);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        //pins
        glTranslatef(0.3, -9.5, 0.05);
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            //back row
            if (PINS[0]) glutSolidCone(0.075, 0.5, 10, 10);
            
            glTranslatef(-0.6/3.0, 0, 0);
            if (PINS[1]) glutSolidCone(0.075, 0.5, 10, 10);

            glTranslatef(-0.6/3.0, 0, 0);
            if (PINS[2]) glutSolidCone(0.075, 0.5, 10, 10);

            glTranslatef(-0.6/3.0, 0, 0);
            if (PINS[3]) glutSolidCone(0.075, 0.5, 10, 10);
            
            //3rd row
            glTranslatef(0.1, 0.1732, 0.0);
            if (PINS[4]) glutSolidCone(0.075, 0.5, 10, 10);

            glTranslatef(0.6/3.0, 0, 0);
            if (PINS[5]) glutSolidCone(0.075, 0.5, 10, 10);

            glTranslatef(0.6/3.0, 0, 0);
            if (PINS[6]) glutSolidCone(0.075, 0.5, 10, 10);

            //2nd row
            glTranslatef(-0.1, 0.1732, 0.0);
            if (PINS[7]) glutSolidCone(0.075, 0.5, 10, 10);

            glTranslatef(-0.6/3.0, 0, 0);
            if (PINS[8]) glutSolidCone(0.075, 0.5, 10, 10);

            //1st row
            glTranslatef(0.1, 0.1732, 0.0);
            if (PINS[9]) glutSolidCone(0.075, 0.5, 10, 10);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        //bowling ball
        glTranslatef(x_pos, -0.5 + y_pos, 0.1);
        glColor3f(0.1, 0.1, 0.1);
        glutSolidSphere(0.1, 10, 10);
	glPopMatrix();

    if (!(WAITING || AIMING)){
        x_pos+=x_vel;
        y_pos+=y_vel;
    }
    if(y_pos <= -9.0) {
        strike_pins();
    }
    if((y_pos <= -10.0) || (x_pos >= 0.6) || (x_pos <= -0.6)) {
        next_turn();
        y_vel = 0;
        y_pos = 0;
        x_vel = 0;
        x_pos = 0;
    }

	glutSwapBuffers();
}

void special(int key, int x, int y){
	switch (key){
	case GLUT_KEY_UP:
		printf("Up Arrow was pressed!\n");
		break;

	case GLUT_KEY_DOWN:
		printf("Down Arrow was pressed!\n");
		break;
	}
}

//Recognizing mouse interaction
void mouse(int btn, int state, int x, int y) {
    //Converting cursor position to proper coordinates
    int relativeY = WINDOW_HEIGHT - y;
    currentX = x;
    currentY = relativeY;
    //printf("Mouse coords: %i,%i\n", x, relativeY);

    if (btn == GLUT_LEFT_BUTTON) {
        printf("Left Mouse Button\n");
        if (AIMING) {

            if (state == GLUT_UP) {     //Left Button released
                throw_ball();
                HOLDING = false;
            }

            if (state == GLUT_DOWN) {   //Left Button held
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

void passive(int x, int y){
	//printf("mousePassive coords: %i,%i\n", x, y);
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w+0.0f)/h), 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPS(int val){
	glutPostRedisplay();
	glutTimerFunc(17, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17
}

//Run at the beginning of runtime, to set up the globals properly
void initialSetup() {
    AIMING = false;
    HOLDING = false;
    ROLLING = false;
    STRIKING = false;
    WAITING = true;
    FRAME_COUNT = 1;
    ROLL_COUNT = 0;
    for (int i = 0; i < 10; i++){ PINS[i] = true; }
    PINS_LEFT = 10;
    CURRENT_PINS_HIT = 0;
    x_pos = 0;
    y_pos = 0;
    startPoint.x = 0; startPoint.y = 0;
    endPoint.x = 0; endPoint.y = 0;
}


void callBackInit(){
	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, FPS, 0);
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("3GC3: Ultra Optimized 60fps Bowling Simulator");	//creates the window

	callBackInit();

	init();

    srand(time(0));
    initialSetup();

    cout << "Welcome to Ultra Optimized 60fps Bowling Simulator!\n";
    cout << "Roll: 1 Frame: 1 Score: 0\n";

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
