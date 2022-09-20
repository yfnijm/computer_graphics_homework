
//Student ID is 0612129. 
#include "glut.h"
#include "math.h"
#include <iostream>
#include <cstdlib>


using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

void drawSun();
void drawEarth();
void drawAxis();
void drawMoon();

GLfloat* sphereVertex(int slice, int stack, GLfloat radius);
void draw(int slice, int stack, GLfloat* color, GLfloat* vertex);
void NormalVertex(GLfloat* a, GLfloat* b, GLfloat* c);

int animation = 1;
int modeNum = 1;
int time = 0;
GLfloat* SunVertex = NULL;
GLfloat* EarthVertex1 = NULL;
GLfloat* EarthVertex2 = NULL;
GLfloat* MoonVertex = NULL;

int width = 400, height = 400;
GLfloat X = 1; //default degree value, you can adjust it
GLfloat Y = 0.5; //default radius value, you can adjust it
const double PI = 3.14159265358979323846;

void lighting()
{
	// enable lighting
	glEnable(GL_LIGHTING);
	//Add directed light
	GLfloat diffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);	    //set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);	    //set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light0
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("HW1");

	lighting();
	glutDisplayFunc(display);

	SunVertex = sphereVertex(240, 60, 7 * Y);
	EarthVertex1 = sphereVertex(360, 180, 2 * Y);
	EarthVertex2 = sphereVertex(4, 2, 2 * Y);
	MoonVertex = sphereVertex(240, 60, Y);
	
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void display()
{   	
	// TO DO: Set the MVP transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 30.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
	
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_CULL_FACE);		//remove back face
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);		//normalized normal 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// TO DO: Draw the Sun, Earth, Moon
	if (animation) {
		time = time++ % (360*28*365);
	}
	drawSun();

	glRotatef(X / 365.0f * time, 0.0f, 0.0f, 1.0f);
	glTranslatef(18.0f, 0.0f, 0.0f);
	glPushMatrix();

	glRotatef(23.5f, 1.0f, 0.0f, 0.0f);
	glRotatef(X / 1.0f * time, 0.0f, 0.0f, 1.0f);
	drawEarth();
	glPushMatrix();

	glPopMatrix();
	glTranslatef(0.0f, 0.0f, -4 * Y);
	drawAxis();
	glPushMatrix();
	
	glPopMatrix();	
	glRotatef(X / 28.0f * time, 0.0f, 0.0f, 1.0f);
	glTranslatef(3.0f, 0.0f, 0.0f);
	glRotatef(X / 28.0f * time , 0.0f, 0.0f, 1.0f);
	drawMoon();

	glutSwapBuffers();
}
void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}


void keyboard(unsigned char key, int x, int y) {
	// TO DO: Set the keyboard control
	switch (key) {
	case 'P':
	case 'p':
		animation = !animation;
		break;
	case 'O':
	case 'o':
		modeNum = !modeNum;
		break;
	default:
		break;
	}
}

void idle() {
		glutPostRedisplay();
}

void drawSun() {
	GLfloat SunColor[3] = { 1.0, 0.0, 0.0 };
	draw(240, 60, SunColor, SunVertex);
}

void drawAxis() {
	glBegin(GL_TRIANGLES);
	GLfloat axisColor[3] = { 0.0, 1.0, 0.0 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, axisColor);
    GLUquadricObj *axisobj = gluNewQuadric();
    gluCylinder(axisobj, 0.1, 0.1, 8 * (GLdouble) Y, 120, 40);
	glEnd();
}

void drawEarth() {
	GLfloat* Earthvertex = (modeNum) ? EarthVertex1 : EarthVertex2;
	int slice = (modeNum) ? 360 : 4;
	int stack = (modeNum) ? 180 : 2;
	GLfloat EarthColor[3] = { 0.0, 0.0, 1.0 };
	draw(slice, stack, EarthColor, Earthvertex);
}

void drawMoon() {
	GLfloat MoonColor[3] = { 0.0, 1.0, 0.0 };
	draw(240, 60, MoonColor,MoonVertex);
}

GLfloat* sphereVertex(int slice, int stack, GLfloat radius) {
	int total_num = 0;

	GLfloat* vertex = (GLfloat*)malloc((2 * slice + 2) * stack * 3 * sizeof(GLfloat));

	for (int stack_num = 0; stack_num < stack; stack_num++) {
		float z = radius - 2 * radius / stack * stack_num;
		float z_n = radius - 2 * radius / stack * (stack_num + 1);
		float stack_r = sqrt(radius * radius - z * z);
		float stack_r_n = sqrt(radius * radius - z_n * z_n);

		if (stack_num == 0) {
			vertex[total_num++] = 0.0;
			vertex[total_num++] = 0.0;
			vertex[total_num++] = z;

			for (int vertexNum = 0; vertexNum < slice + 1; vertexNum++) {
				vertex[total_num++] = stack_r_n * cos((2 * PI / slice) * vertexNum);
				vertex[total_num++] = stack_r_n * sin((2 * PI / slice) * vertexNum);
				vertex[total_num++] = z_n;
			}
		}
		else if (stack_num == (stack - 1)) {
			vertex[total_num++] = 0.0;
			vertex[total_num++] = 0.0;
			vertex[total_num++] = z_n;

			for (int vertexNum = 0; vertexNum < slice + 1; vertexNum++) {
				vertex[total_num++] = stack_r * cos((-2 * PI / slice) * vertexNum);
				vertex[total_num++] = stack_r * sin((-2 * PI / slice) * vertexNum);
				vertex[total_num++] = z;
			}
		}
		else {
			for (int vertexNum = 0; vertexNum < slice + 1; vertexNum++){   
				vertex[total_num++] = stack_r * cos((2 * PI / slice) * vertexNum);
				vertex[total_num++] = stack_r * sin((2 * PI / slice) * vertexNum);
				vertex[total_num++] = z;

				vertex[total_num++] = stack_r_n * cos((2 * PI / slice) * vertexNum);
				vertex[total_num++] = stack_r_n * sin((2 * PI / slice) * vertexNum);
				vertex[total_num++] = z_n;
			}
		}
	}
	return vertex;
}

void draw(int slice, int stack, GLfloat* color, GLfloat* vertex) {
	int vertexNum = 0;
	for (int i = 0; i < stack; i++) {
		if (i == 0 || i == stack - 1) {
			glBegin(GL_TRIANGLE_FAN);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			GLfloat *mainVertex = &vertex[vertexNum];
			for (int j = 0; j < slice + 2; j++) {
				
				if (j == 0) NormalVertex(mainVertex, &vertex[vertexNum + 3], &vertex[vertexNum + 6]);
				else if (j == 1)NormalVertex(&vertex[vertexNum], &vertex[vertexNum + 3], mainVertex);
				else if (j == 2)NormalVertex(&vertex[vertexNum], mainVertex, &vertex[vertexNum - 3]);
				else NormalVertex(&vertex[vertexNum], mainVertex, &vertex[vertexNum - 3]);			
				
				glVertex3f(vertex[vertexNum], vertex[vertexNum + 1], vertex[vertexNum + 2]);
				vertexNum += 3;
			}
			glEnd();
		}
		else {
			glBegin(GL_TRIANGLE_STRIP);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			for (int j = 0; j < slice * 2 + 2; j++) {
				if (!j) NormalVertex(&vertex[vertexNum], &vertex[vertexNum + 3], &vertex[vertexNum + 6]);
				else if (j == 1) NormalVertex(&vertex[vertexNum], &vertex[vertexNum + 3], &vertex[vertexNum - 3]);
				else if (j == 2) NormalVertex(&vertex[vertexNum], &vertex[vertexNum - 6], &vertex[vertexNum - 3]);
				else {
					if (!(j % 2))NormalVertex(&vertex[vertexNum], &vertex[vertexNum - 6], &vertex[vertexNum - 3]);
					else NormalVertex(&vertex[vertexNum], &vertex[vertexNum - 3], &vertex[vertexNum - 6]);
				}
				
				glVertex3f(vertex[vertexNum], vertex[vertexNum + 1], vertex[vertexNum + 2]);
				vertexNum += 3;
			}
			glEnd();
		}
	}
}

void NormalVertex(GLfloat *v1, GLfloat *v2, GLfloat *v3) {
	GLfloat a[3], b[3];
	a[0] = v2[0] - v1[0]; b[0] = v3[0] - v1[0];
	a[1] = v2[1] - v1[1]; b[1] = v3[1] - v1[1];
	a[2] = v2[2] - v1[2]; b[2] = v3[2] - v1[2];
	glNormal3f(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}