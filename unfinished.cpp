#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

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
float cpos[] = {0, 0, 0};
float cangle[] = {0, 0};
float t[3145728];
GLuint tex[2];
string texnames[2] = {"grass.ppm", "blocks1.ppm"};

class Point{
	float x;
	float y;
	float z;
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
		texid = i + 1;
	}
	int gettexid(){
		return texid;
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
		ifstream f(file);
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
		//points.shrink_to_fit();
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
};

Model m;

void loadtexture(string k){
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glGenTextures(2, tex);
	for(int i = 0; i < 2; i++){
		loadtexture(texnames[i]);
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_FLOAT, t);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	m.setmodel("model");
}

void reshape(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int xin, int yin){
	if(key == 'w'){
		cpos[0] = cpos[0] + (sin(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] + (cos(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 's'){
		cpos[0] = cpos[0] - (sin(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] - (cos(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'a'){
		cpos[0] = cpos[0] + (cos(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] - (sin(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'd'){
		cpos[0] = cpos[0] - (cos(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] + (sin(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'q'){
		cpos[1] = cpos[1] + 0.5;
	}else if(key == 'e'){
		cpos[1] = cpos[1] - 0.5;
	}
	glutPostRedisplay();
}

void special(int key, int xin, int yin){
	if(key == GLUT_KEY_UP){
		if(cangle[0] < 90){
			cangle[0] = cangle[0] + 2;
		}
	}else if(key == GLUT_KEY_DOWN){
		if(cangle[0] > -90){
			cangle[0] = cangle[0] - 2;
		}
	}else if(key == GLUT_KEY_LEFT){
		cangle[1] = cangle[1] + 2;
	}else if(key == GLUT_KEY_RIGHT){
		cangle[1] = cangle[1] - 2;
	}
	if(cangle[1] < 0.0){
		cangle[1] += 360.0;
	}else if(cangle[1] >= 360.0){
		cangle[1] -= 360.0;
	}
	glutPostRedisplay();
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(-cangle[0], 1, 0, 0);
	glRotatef(-cangle[1], 0, 1, 0);
	glTranslatef(cpos[0], cpos[1], cpos[2]);
	glPushMatrix();
	glTranslatef(0, 0, -5);
	for(int i = 0; i < m.tril(); i++){
		glBindTexture(GL_TEXTURE_2D, (*m.gettri(i)).gettexid());
		glBegin(GL_TRIANGLES);
		glTexCoord2f((*m.gettri(i)).getuv(0, 0), (*m.gettri(i)).getuv(0, 1));
		glVertex3f((*(*m.gettri(i)).gete1()).getx(), (*(*m.gettri(i)).gete1()).gety(), (*(*m.gettri(i)).gete1()).getz());
		glTexCoord2f((*m.gettri(i)).getuv(1, 0), (*m.gettri(i)).getuv(1, 1));
		glVertex3f((*(*m.gettri(i)).gete2()).getx(), (*(*m.gettri(i)).gete2()).gety(), (*(*m.gettri(i)).gete2()).getz());
		glTexCoord2f((*m.gettri(i)).getuv(2, 0), (*m.gettri(i)).getuv(2, 1));
		glVertex3f((*(*m.gettri(i)).gete3()).getx(), (*(*m.gettri(i)).gete3()).gety(), (*(*m.gettri(i)).gete3()).getz());
		glEnd();
	}
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);
	winid = glutCreateWindow("");
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return(0);
}
