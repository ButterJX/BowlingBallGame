#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <math.h>

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

//program state flags
bool AIMING;
bool ROLLING;
bool STRIKING;

//pin state values
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
float bweight = 4.0;
float pweight = 0.5;

//Globals
float throwCamPos[] = {0.0f, -2.0f, 1.0f};  //where the camera is when throwing
float pinCamPos[] = {0.0f, -8.5f, 1.5f};    //where the camera is when hitting pins
float ballCamPos[] = {0.8f, 1.8f, 0.8f};    //follow postion wrt ball

float original_positions[] = {
    0.3,    -9.5,
    0.1,    -9.5,
    -0.1,   -9.5,
    -0.3,   -9.5,
    0.2,    -9.3268,
    0.0,    -9.3268,
    -0.2,   -9.3268,
    0.1,    -9.1536,
    -0.1,   -9.1536,
    0.0,    -8.9804
};

class Pin{
    public:
    float original_x, original_y;
    float x, y;
    float vel_x, vel_y;
    bool present;
};

Pin PINS[10];

void throw_ball(float* fl){
    AIMING = false;
    ROLLING = true;
    float angle = atan2(fl[2], fl[1]);
    y_vel = fl[0] * 0.001 * -sin(angle);//0.06 * (double) (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.1;
    x_vel = fl[0] * 0.001 * cos(angle);//0.002 * (double) (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.001;
    printf("%f, %f\n",x_vel,y_vel);
}

void next_turn(){
    STRIKING = false;
    ROLLING = false;
    AIMING = true;
    x_pos = y_pos = x_vel = y_vel = 0;


    for (int i = 0; i < 10; i++){
        if (PINS[i].present && (PINS[i].x != PINS[i].original_x || PINS[i].y != PINS[i].original_y)){
            PINS[i].present = false;
            CURRENT_PINS_HIT++;
        }
    }
    PINS_LEFT -= CURRENT_PINS_HIT;
    
    //for determining the next frame and roll
    if (FRAME_COUNT < 10){
        if (ROLL_COUNT == 0){
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins! " + to_string(PINS_LEFT) + " remain this frame.\n";
            ROLL_COUNT++;
        }
        else{
            cout << "You hit " +  to_string(CURRENT_PINS_HIT) +  " pins and " + to_string(10 - PINS_LEFT) + " in total this frame.\n";
            ROLL_COUNT = 0;
            for (int i = 0; i < 10; i++){ 
                PINS[i].present = true; 
                PINS[i].x = PINS[i].original_x;
                PINS[i].y = PINS[i].original_y;
                PINS[i].vel_x = 0;
                PINS[i].vel_y = 0;
            }
            FRAME_COUNT++;
            PINS_LEFT = 10;
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
            for (int i = 0; i < 10; i++){ 
                PINS[i].present = true; 
                PINS[i].x = PINS[i].original_x;
                PINS[i].y = PINS[i].original_y;
                PINS[i].vel_x = 0;
                PINS[i].vel_y = 0;
            }
            FRAME_COUNT++;
            PINS_LEFT = 10;
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
            for (int i = 0; i < 10; i++){ 
                PINS[i].present = true; 
                PINS[i].x = PINS[i].original_x;
                PINS[i].y = PINS[i].original_y;
                PINS[i].vel_x = 0;
                PINS[i].vel_y = 0;
            }
            PINS_LEFT = 10;
        }
        else {
            DOUBLE_POINTS = 1;
            cout << "SPARE!\n";
            if (FRAME_COUNT == 10 && ROLL_COUNT == 2){
                for (int i = 0; i < 10; i++){ 
                    PINS[i].present = true; 
                    PINS[i].x = PINS[i].original_x;
                    PINS[i].y = PINS[i].original_y;
                    PINS[i].vel_x = 0;
                    PINS[i].vel_y = 0;
                }
            }
            PINS_LEFT = 10;
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

void checkForEnd(){
    bool END_TURN = true;
    for (int i = 0; i < 10; i++){
        if (PINS[i].y < -10.0) PINS[i].present = false; 
        if (PINS[i].vel_x > 0.000005 || PINS[i].vel_x < -0.000005 || PINS[i].vel_y > 0.000005 || PINS[i].vel_y < -0.000005 || !PINS[i].present){
            printf("pin %i still moving\n", i);
            END_TURN = false;
            break;
        }
    }
    if (END_TURN) next_turn();
}

void collision(){
    
    for (int i = 0; i < 11; i++){
        for (int j = 1; j < 10; j++){
            if (i == 10){
                //printf("%f\n",sqrt ( pow(x_pos - PINS[j].x, 2) + pow(y_pos - PINS[j].y,2) ));
                if ( (sqrt ( pow(x_pos - PINS[j].x, 2) + pow(y_pos - PINS[j].y,2) ) < 0.15) &&
                    PINS[j].present) {
                    if (!STRIKING) { STRIKING = true; ROLLING = false; }
                    //v1f = (m1*v1i + 2*m2*v2i - m2*v1i) / (m1 + m2)
                    //( bweight*x_vel + 2*pweight*PINS[j].vel_x - pweight*x_vel ) / (pweight + bweight);
                    float vel_mag_ball = sqrt( pow(x_vel,2) + pow(y_vel,2) );
                    float vel_mag_pin =  sqrt( pow(PINS[j].vel_x,2) + pow(PINS[j].vel_x,2) );
                    float theta_ball = atan2(y_vel, x_vel);
                    float theta_pin = atan2(PINS[j].vel_y, PINS[j].vel_x);
                    float phi = atan2( PINS[j].y - y_pos, PINS[j].x -x_pos ); 
                    float ball_x =  ((vel_mag_ball * cos(theta_ball - phi) * (bweight - pweight) + 2 * pweight * vel_mag_pin * cos( theta_pin - phi )) * cos(phi) /
                                    (pweight + bweight)) + vel_mag_ball * sin(theta_ball - phi) * sin(phi);
                    float ball_y =  ((vel_mag_ball * cos(theta_ball - phi) * (bweight - pweight) + 2 * pweight * vel_mag_pin * cos( theta_pin - phi )) * sin(phi) /
                                    (pweight + bweight)) + vel_mag_ball * sin(theta_ball - phi) * cos(phi);
                    float pin_x =   ((vel_mag_pin * cos(theta_pin - phi) * (pweight - bweight) + 2 * bweight * vel_mag_ball * cos( theta_ball - phi )) * cos(phi) /
                                    (pweight + bweight)) + vel_mag_pin * sin(theta_pin - phi) * sin(phi);
                    float pin_y =   ((vel_mag_pin * cos(theta_pin - phi) * (pweight - bweight) + 2 * bweight * vel_mag_ball * cos( theta_ball - phi )) * sin(phi) /
                                    (pweight + bweight)) + vel_mag_pin * sin(theta_pin - phi) * cos(phi);
                    x_vel = ball_x;
                    y_vel = ball_y;
                    PINS[j].vel_x = pin_x;
                    PINS[j].vel_y = pin_y;
                }
            }
            else if (i < j) {
                if ( (sqrt ( pow(PINS[i].x - PINS[j].x, 2) + pow(PINS[i].y - PINS[j].y , 2) ) < 0.1) &&
                    PINS[i].present && PINS[j].present){
                    if (!STRIKING) { STRIKING = true; ROLLING = false; }
                    //v1f = (m1*v1i + 2*m2*v2i - m2*v1i) / (m1 + m2)
                    float vel_mag_pin1 =  sqrt( pow(PINS[i].vel_x,2) + pow(PINS[i].vel_x,2) );
                    float vel_mag_pin2 =  sqrt( pow(PINS[j].vel_x,2) + pow(PINS[j].vel_x,2) );
                    float theta_pin1 = atan2(PINS[i].vel_y, PINS[i].vel_x);
                    float theta_pin2 = atan2(PINS[j].vel_y, PINS[j].vel_x);
                    float phi = atan2( PINS[j].y - y_pos, PINS[j].x -x_pos ); 
                    float pin1_x =  ((vel_mag_pin1 * cos(theta_pin1 - phi) * (pweight - pweight) + 2 * pweight * vel_mag_pin2 * cos( theta_pin2 - phi )) * cos(phi) /
                                    (pweight + bweight)) + vel_mag_pin1 * sin(theta_pin1 - phi) * sin(phi);
                    float pin1_y =  ((vel_mag_pin1 * cos(theta_pin1 - phi) * (pweight - pweight) + 2 * pweight * vel_mag_pin2 * cos( theta_pin2 - phi )) * sin(phi) /
                                    (pweight + bweight)) + vel_mag_pin1 * sin(theta_pin1 - phi) * cos(phi);
                    float pin2_x =  ((vel_mag_pin2 * cos(theta_pin2 - phi) * (pweight - pweight) + 2 * pweight * vel_mag_pin1 * cos( theta_pin1 - phi )) * cos(phi) /
                                    (pweight + pweight)) + vel_mag_pin2 * sin(theta_pin2 - phi) * sin(phi);
                    float pin2_y =  ((vel_mag_pin2 * cos(theta_pin2 - phi) * (pweight - pweight) + 2 * pweight * vel_mag_pin1 * cos( theta_pin1 - phi )) * sin(phi) /
                                    (pweight + pweight)) + vel_mag_pin2 * sin(theta_pin2 - phi) * cos(phi);
                    PINS[i].vel_x = pin1_x;
                    PINS[i].vel_y = pin1_y;
                    PINS[j].vel_x = pin2_x;
                    PINS[j].vel_y = pin2_y;
                }
            }
        }
    }
}

//OpenGL functions
void keyboardl(unsigned char key, int xIn, int yIn)
{
    int mod = glutGetModifiers();
    switch (key)
    {
        /*case 't':
            if (AIMING) throw_ball();
            break;*/
        case 'q':
        case 27:
            exit(0);
            break;
        case 'e':
            if (STRIKING) next_turn();
            break;
    }
}

/*void init(void)
{
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
}*/

/* display function - GLUT display callback function
 * clears the screen, sets the camera position, draws the ground plane and movable box
 */
void displayl(void)
{
    /*glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/

    if (AIMING) {
        gluLookAt(throwCamPos[0] + x_pos, throwCamPos[2], throwCamPos[2] + y_pos + 1.0, 0.0, 0.0, -9.5, 0,1,0);
    }else if ((ROLLING && y_pos < -7.5) || STRIKING){
        gluLookAt(pinCamPos[0], pinCamPos[2], pinCamPos[1], 0.0, 0.0, -9.5, 0,1,0);
    }else if (ROLLING){
        gluLookAt(ballCamPos[0] + x_pos, ballCamPos[2], ballCamPos[1] + y_pos, x_pos, 0.0, y_pos, 0,1,0);
    }
}
    /*glPushMatrix();
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
        glTranslatef(0, 0, 0.05);
        glColor3f(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 10; i++){    
            glPushMatrix();
                if (PINS[i].present){ 
                    glTranslatef(PINS[i].x, PINS[i].y, 0);
                    glutSolidCone(0.05, 0.4, 10, 10);
                }
            glPopMatrix();
        }
    glPopMatrix();

    glPushMatrix();
        //bowling ball
        glTranslatef(x_pos, -0.5 + y_pos, 0.1);
        glColor3f(0.1, 0.1, 0.1);
        glutSolidSphere(0.1, 10, 10);
    glPopMatrix();*/
void updatel(){

    x_pos+=x_vel;
    y_pos+=y_vel;

    for (int i = 0; i < 10; i++){
        PINS[i].x += PINS[i].vel_x;
        PINS[i].y += PINS[i].vel_y;
        if (PINS[i].vel_x > 0.0000005) PINS[i].vel_x -= 0.0000005;
        if (PINS[i].vel_y > 0.0000005) PINS[i].vel_y -= 0.0000005;
        if (PINS[i].vel_x < -0.0000005) PINS[i].vel_x += 0.0000005;
        if (PINS[i].vel_y < -0.0000005) PINS[i].vel_y += 0.0000005;
    }

    if (!AIMING) collision();

    if (STRIKING)
        checkForEnd();
    
    if(y_pos <= -10.0 || x_pos > 2 || x_pos < -2 || y_pos > 1) {
        printf("y_pos: %f, x_pos: %f", y_pos, x_pos);
        y_vel = 0;
        x_vel = 0;
        x_pos = 0;
        y_pos = 0;
        if (!STRIKING && !AIMING){
            next_turn();
        }
    }

    //glutSwapBuffers();
}

void speciall(int key, int x, int y){
    switch (key){
    case GLUT_KEY_UP:
        printf("Up Arrow was pressed!\n");
        break;

    case GLUT_KEY_DOWN:
        printf("Down Arrow was pressed!\n");
        break;
    }
}

void mousel(int btn, int state, int x, int y){
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        //mouse controls if any
        cout << "mouse memes\n";
    }
}

void motionl(int x, int y){
    //printf("mouseMotion coords: %i,%i\n", x, y);
}

void passivel(int x, int y){
    //printf("mousePassive coords: %i,%i\n", x, y);
}

/*void reshape(int w, int h)
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

void callBackInit(){
    glutDisplayFunc(display);   //registers "display" as the display callback function
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passive);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, FPS, 0);
}

/* main function - program entry point
int main(int argc, char** argv)
{
    glutInit(&argc, argv);      //starts up GLUT
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("3GC3: Ultra Optimized 60fps Bowling Simulator (now with Collision!)");    //creates the window*/

    //callBackInit();

void initl(){

    srand(time(0));
    AIMING = true;
    ROLLING = false;
    STRIKING = false;
    FRAME_COUNT = 1;
    ROLL_COUNT = 0;
    for (int i = 0; i < 10; i++){ 
        PINS[i].present = true; 
        PINS[i].original_x = PINS[i].x = original_positions[2*i];
        PINS[i].original_y = PINS[i].y = original_positions[2*i+1];
        PINS[i].vel_x = 0;
        PINS[i].vel_y = 0;
    }
    PINS_LEFT = 10;
    CURRENT_PINS_HIT = 0;
    x_pos = 0;
    y_pos = 0;
    cout << "Welcome to Ultra Optimized 60fps Bowling Simulator!\n";
    cout << "Roll: 1 Frame: 1 Score: 0\n";

    //glutMainLoop();               //starts the event glutMainLoop
    //return(0);                    //return may not be necessary on all compilers
}
