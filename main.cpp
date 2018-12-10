#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "logic.h"
#include "throwing.h"

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

const float pi = 3.14159;

int winid;
float cpos[] = {0.0, 1.5, 0.0};//camera position
float cangle[] = {0, 0};//camera angle
float l1pos[] = {0, 5, 0, 1};//position of lights
float l2pos[] = {0, 5, -6, 1};
float amb[] = {0.2, 0.2, 0.2, 1.0};
float dif[] = {0.8, 0.8, 0.8, 1.0};
float spec[] = {0.85, 0.85, 0.85, 1.0};
float l1amb[] = {0.2, 0.2, 0.2, 1.0};
float l1dif[] = {0.8, 0.8, 0.8, 1.0};
float l1spec[] = {0.85, 0.85, 0.85, 1.0};
float ballamb[] = {0.05, 0.05, 0.05, 1.0};
float balldif[] = {0.1, 0.1, 0.1, 1.0};
float ballspec[] = {0.5, 0.5, 0.5, 1.0};
float pinamb[] = {0.2, 0.2, 0.2, 1.0};
float pindif[] = {0.8, 0.8, 0.8, 1.0};
float pinspec[] = {0.85, 0.85, 0.85, 1.0};
float t[3145728];//buffer for texture data
bool m1 = false;
GLuint tex[2];
string texnames[2] = {"wood.ppm", "wood2.ppm"};

class Point{
	float x, y, z;
	float norm[3];
	public:
	void set(float a, float b, float c){
		x = a;
		y = b;
		z = c;
	}
	float getx(){
		return x;
	}
	float gety(){
		return y;
	}
	float getz(){
		return z;
	}
	void setnormx(float a){
		norm[0] = a;
	}
	float getnormx(){
		return norm[0];
	}
	void setnormy(float a){
		norm[1] = a;
	}
	float getnormy(){
		return norm[1];
	}
	void setnormz(float a){
		norm[2] = a;
	}
	float getnormz(){
		return norm[2];
	}
};

class Triangle{
	Point* e1;
	Point* e2;
	Point* e3;
	float uv[3][2];
	int texid;
	float norm[3];
	public:
	void sete1(Point* a){
		e1 = a;
	}
	void sete2(Point* a){
		e2 = a;
	}
	void sete3(Point* a){
		e3 = a;
	}
	Point* gete1(){
		return e1;
	}
	Point* gete2(){
		return e2;
	}
	Point* gete3(){
		return e3;
	}
	void setuv(float a, int i, int j){
		uv[i][j] = a;
	}
	float getuv(int i, int j){
		return uv[i][j];
	}
	void settexid(string a){
		int i = 0;
		while(a.compare(texnames[i]) != 0){
			i++;
		}
		texid = i;
	}
	int gettexid(){
		return texid;
	}
	void setnormx(float a){
		norm[0] = a;
	}
	float getnormx(){
		return norm[0];
	}
	void setnormy(float a){
		norm[1] = a;
	}
	float getnormy(){
		return norm[1];
	}
	void setnormz(float a){
		norm[2] = a;
	}
	float getnormz(){
		return norm[2];
	}
};

class Model{
	vector<Point> points;
	vector<Triangle> tri;
	void getcoords(string k, float* u1, float* u2, float* u3){
		int h1;
		int h2;
		string a;
		string b;
		string c;
		h1 = k.find_first_of(",");
		h2 = k.find_last_of(",");
		a = k.substr(0, h1);
		b = k.substr(h1 + 2, h2 - (h1 + 2));
		c = k.substr(h2 + 2, k.length() - (h2 + 2));
		*u1 = stof(a);
		*u2 = stof(b);
		*u3 = stof(c);
	}
	void getcoords2(string k, float* u1, float* u2){
		int h;
		string a;
		string b;
		h = k.find_first_of(",");
		a = k.substr(0, h);
		b = k.substr(h + 2, k.length() - (h + 2));
		*u1 = stof(a);
		*u2 = stof(b);
	}
	public:
	void setmodel(string file){
		int h1;
		int h2;
		int vecpos;
		int r;
		float u[3];
		float u2[2];
		bool isinvec = false;
		string a;
		string c[3];
		string uv[3];
		string t;
		ifstream f(file);//loading model file
		getline(f, a);
		r = stoi(a);
		points.reserve(r * 3);
		for(int i = 0; i < r; i++){
			getline(f, a);
			h1 = a.find_first_of(";");
			h2 = a.find_last_of(";");
			c[0] = a.substr(0, h1);
			c[1] = a.substr(h1 + 2, h2 - (h1 + 2));
			c[2] = a.substr(h2 + 2, a.length() - (h2 + 2));
			getline(f, a);
			h1 = a.find_first_of(";");
			h2 = a.find_last_of(";");
			uv[0] = a.substr(0, h1);
			uv[1] = a.substr(h1 + 2, h2 - (h1 + 2));
			uv[2] = a.substr(h2 + 2, a.length() - (h2 + 2));
			getline(f, t);
			tri.push_back(Triangle());
			for(int j = 0; j < 3; j++){
				getcoords(c[j], &u[0], &u[1], &u[2]);
				getcoords2(uv[j], &u2[0], &u2[1]);
				for(int k = 0; k < points.size(); k++){
					if(u[0] == points[k].getx() && u[1] == points[k].gety() && u[2] == points[k].getz()){
						isinvec = true;
						vecpos = k;
					}
				}
				if(isinvec == false){
					points.push_back(Point());
					points.back().set(u[0], u[1], u[2]);
					if(j == 0){
						tri.back().sete1(&points.back());
					}else if(j == 1){
						tri.back().sete2(&points.back());
					} else {
						tri.back().sete3(&points.back());
					}
				} else {
					if(j == 0){
						tri.back().sete1(&points[vecpos]);
					}else if(j == 1){
						tri.back().sete2(&points[vecpos]);
					} else {
						tri.back().sete3(&points[vecpos]);
					}
				}
				tri.back().setuv(u2[0], j, 0);
				tri.back().setuv(u2[1], j, 1);
				isinvec = false;
			}
			tri.back().settexid(t);
		}
		f.close();
	}
	Triangle* gettri(int i){
		return &(tri[i]);
	}
	void pushtri(Triangle t){
		tri.push_back(t);
	}
	int tril(){
		return tri.size();
	}
	Point* getpoint(int i){
		return &(points[i]);
	}
	void pushpoint(Point p){
		points.push_back(p);
	}
	int pointsl(){
		return points.size();
	}
	void setnorms(){
		float n1[3];
		float n2[3];
		float h1, h2, h3, h4;
		for(int i = 0; i < tri.size(); i++){//calculate face normals
			n1[0] = (*tri[i].gete2()).getx() - (*tri[i].gete1()).getx();
			n1[1] = (*tri[i].gete2()).gety() - (*tri[i].gete1()).gety();
			n1[2] = (*tri[i].gete2()).getz() - (*tri[i].gete1()).getz();
			n2[0] = (*tri[i].gete3()).getx() - (*tri[i].gete1()).getx();
			n2[1] = (*tri[i].gete3()).gety() - (*tri[i].gete1()).gety();
			n2[2] = (*tri[i].gete3()).getz() - (*tri[i].gete1()).getz();
			h1 = (n1[1] * n2[2]) - (n1[2] * n2[1]);
			h2 = (n1[2] * n2[0]) - (n1[0] * n2[2]);
			h3 = (n1[0] * n2[1]) - (n1[1] * n2[0]);
			h4 = sqrt(pow(h1, 2.0) + pow(h2, 2.0) + pow(h3, 2.0));
			tri[i].setnormx(h1 / h4);
			tri[i].setnormy(h2 / h4);
			tri[i].setnormz(h3 / h4);
		}
		for(int i = 0; i < points.size(); i++){//calculate vertex normals, lighting calculations use these
			h1 = 0;
			h2 = 0;
			h3 = 0;
			for(int j = 0; j < tri.size(); j++){
				if(tri[j].gete1() == &points[i] || tri[j].gete2() == &points[i] || tri[j].gete3() == &points[i]){
					h1 += tri[j].getnormx();
					h2 += tri[j].getnormy();
					h3 += tri[j].getnormz();
				}
			}
			h4 = sqrt(pow(h1, 2.0) + pow(h2, 2.0) + pow(h3, 2.0));
			points[i].setnormx(h1 / h4);
			points[i].setnormy(h2 / h4);
			points[i].setnormz(h3 / h4);
		}
	}
};

class Object{
	float pos[3] = {0.0, 0.0, 0.0};
	float angle[3] = {0.0, 0.0, 0.0};
	float scale[3] = {1.0, 1.0, 1.0};
	int type;//0 = lane, 1 = sphere
	Model* m;
	public:
	void setposx(float a){
		pos[0] = a;
	}
	float getposx(){
		return pos[0];
	}
	void setposy(float a){
		pos[1] = a;
	}
	float getposy(){
		return pos[1];
	}
	void setposz(float a){
		pos[2] = a;
	}
	float getposz(){
		return pos[2];
	}
	void setanglex(float a){
		angle[0] = a;
	}
	float getanglex(){
		return angle[0];
	}
	void setangley(float a){
		angle[1] = a;
	}
	float getangley(){
		return angle[1];
	}
	void setanglez(float a){
		angle[2] = a;
	}
	float getanglez(){
		return angle[2];
	}
	void setscalex(float a){
		scale[0] = a;
	}
	float getscalex(){
		return scale[0];
	}
	void setscaley(float a){
		scale[1] = a;
	}
	float getscaley(){
		return scale[1];
	}
	void setscalez(float a){
		scale[2] = a;
	}
	float getscalez(){
		return scale[2];
	}
	void setmodel(Model* a){
		m = a;
	}
	Model* getmodel(){
		return m;
	}
	void settype(int a){
		type = a;
	}
	int gettype(){
		return type;
	}
};

vector<Model> models;
vector<Object> objects;

void loadtexture(string k){//loads texture from file
	int h;
	string a;
	string b;
	ifstream f(k);
	getline(f, a);
	getline(f, a);
	getline(f, a);
	h = a.find_first_of(",");
	b = a.substr(0, h);
	h = stoi(b);
	getline(f, b);
	for(int i = 0; i < h * h * 3; i++){
		getline(f, b);
		t[i] = (float)stoi(b) / 255.0;
	}
	f.close();
}

void init(void){
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l1dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1spec);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glGenTextures(2, tex);
	for(int i = 0; i < 2; i++){//loading textures into vram
		loadtexture(texnames[i]);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_FLOAT, t);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	models.push_back(Model());
	models.back().setmodel("lane");
	models.back().setnorms();
	objects.push_back(Object());//create objects
	objects.back().settype(0);//lane
	objects.back().setscalez(0.5);
	objects.back().setmodel(&models[0]);
	objects.push_back(Object());//ball
	objects.back().setposx(x_pos);
	objects.back().setposy(0.1);
	objects.back().setposz(y_pos);
	objects.back().settype(1);
	for(int i = 0; i < 10; i++){
		objects.push_back(Object());//pins
		objects.back().setposx(PINS[i].x);
		objects.back().setposz(PINS[i].y);
		objects.back().settype(2);
	}
}

void reshape(int w, int h){
	WINDOW_WIDTH = w;
	WINDOW_HEIGHT = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int xin, int yin){
	keyboardl(key, xin, yin);
}

void special(int key, int xin, int yin){
	speciall(key, xin, yin);
}

void mouse(int btn, int state, int x, int y){
	mousel(btn, state, x, y);
	if(AIMING == true){
		if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
			start(x, WINDOW_HEIGHT - y);
			m1 = true;
		}else if(btn == GLUT_LEFT_BUTTON && state == GLUT_UP){
			float fl[3];
			release(x, WINDOW_HEIGHT - y, fl);
			throw_ball(fl);
			m1 = false;
		}
	}
}

void motion(int x, int y){
	motionl(x, y);
	setPos(x, WINDOW_HEIGHT - y);
}

void passive(int x, int y){
	passivel(x, y);
	setPos(x, WINDOW_HEIGHT - y);
}

void update(int){
	objects[1].setposx(x_pos);//update ball position
	objects[1].setposz(y_pos);
	objects[1].setanglex(objects[1].getanglex() + (y_vel * 50.0));//causes ball to appear to roll
	objects[1].setanglez(objects[1].getanglez() - (x_vel * 50.0));
	for(int i = 0; i < 10; i++){//update pin positions
		objects[i + 2].setposx(PINS[i].x);
		objects[i + 2].setposz(PINS[i].y);
	}
	collision();//check for collisions
	updatel();
	if(y_pos > 1.0 || y_pos < -20.0){
		next_turn();
	}
	if(x_pos > 1.5 || x_pos < -1.5){
		x_vel = 0.0;
	}
	glutPostRedisplay();
	glutTimerFunc(1000.0 / 60.0, update, 0);
}

void drawobject(int i){//draw model to screen
	for(int j = 0; j < (*objects[i].getmodel()).tril(); j++){
		glBindTexture(GL_TEXTURE_2D, tex[(*(*objects[i].getmodel()).gettri(j)).gettexid()]);
		glBegin(GL_TRIANGLES);
		glTexCoord2f((*(*objects[i].getmodel()).gettri(j)).getuv(0, 0), (*(*objects[i].getmodel()).gettri(j)).getuv(0, 1));
		glNormal3f((*(*(*objects[i].getmodel()).gettri(j)).gete1()).getnormx(), (*(*(*objects[i].getmodel()).gettri(j)).gete1()).getnormy(), (*(*(*objects[i].getmodel()).gettri(j)).gete1()).getnormz());
		glVertex3f((*(*(*objects[i].getmodel()).gettri(j)).gete1()).getx(), (*(*(*objects[i].getmodel()).gettri(j)).gete1()).gety(), (*(*(*objects[i].getmodel()).gettri(j)).gete1()).getz());
		glTexCoord2f((*(*objects[i].getmodel()).gettri(j)).getuv(1, 0), (*(*objects[i].getmodel()).gettri(j)).getuv(1, 1));
		glNormal3f((*(*(*objects[i].getmodel()).gettri(j)).gete2()).getnormx(), (*(*(*objects[i].getmodel()).gettri(j)).gete2()).getnormy(), (*(*(*objects[i].getmodel()).gettri(j)).gete2()).getnormz());
		glVertex3f((*(*(*objects[i].getmodel()).gettri(j)).gete2()).getx(), (*(*(*objects[i].getmodel()).gettri(j)).gete2()).gety(), (*(*(*objects[i].getmodel()).gettri(j)).gete2()).getz());
		glTexCoord2f((*(*objects[i].getmodel()).gettri(j)).getuv(2, 0), (*(*objects[i].getmodel()).gettri(j)).getuv(2, 1));
		glNormal3f((*(*(*objects[i].getmodel()).gettri(j)).gete3()).getnormx(), (*(*(*objects[i].getmodel()).gettri(j)).gete3()).getnormy(), (*(*(*objects[i].getmodel()).gettri(j)).gete3()).getnormz());
		glVertex3f((*(*(*objects[i].getmodel()).gettri(j)).gete3()).getx(), (*(*(*objects[i].getmodel()).gettri(j)).gete3()).gety(), (*(*(*objects[i].getmodel()).gettri(j)).gete3()).getz());
		glEnd();
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	displayl();
	glMaterialfv(GL_FRONT, GL_AMBIENT, l1amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, l1dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT0, GL_POSITION, l1pos);
	glLightfv(GL_LIGHT1, GL_POSITION, l2pos);
	for(int i = 0; i < objects.size(); i++){
		glPushMatrix();
		glTranslatef(objects[i].getposx(), objects[i].getposy(), objects[i].getposz());
		glRotatef(objects[i].getangley(), 0.0, 1.0, 0.0);
		glRotatef(objects[i].getanglex(), 1.0, 0.0, 0.0);
		glRotatef(objects[i].getanglez(), 0.0, 0.0, 1.0);
		glScalef(objects[i].getscalex(), objects[i].getscaley(), objects[i].getscalez());
		if(objects[i].gettype() == 0){
			drawobject(i);
		}else if(objects[i].gettype() == 1){
			glDisable(GL_TEXTURE_2D);
			glMaterialfv(GL_FRONT, GL_AMBIENT, ballamb);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, balldif);
			glMaterialfv(GL_FRONT, GL_SPECULAR, ballspec);
			glutSolidSphere(0.1, 10, 10);
			glEnable(GL_TEXTURE_2D);
			
		}else if(objects[i].gettype() == 2){
			glPushMatrix();
			glRotatef(-90.0, 1, 0, 0);
			glDisable(GL_TEXTURE_2D);
			glMaterialfv(GL_FRONT, GL_AMBIENT, pinamb);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, pindif);
			glMaterialfv(GL_FRONT, GL_SPECULAR, pinspec);
			glutSolidCylinder(0.05, 0.4, 10, 10);
			glEnable(GL_TEXTURE_2D);
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
	if(m1 == true){
		drawPowerLine();//draw the line
	}
	glutSwapBuffers();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	winid = glutCreateWindow("3GC3: Ultra Optimized 60fps Bowling Simulator (now with Collision!)");
	glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive);
	glutTimerFunc(1000.0 / 60.0, update, 0);
	glutDisplayFunc(display);
	initl();
	init();
	glutMainLoop();
	return(0);
}
