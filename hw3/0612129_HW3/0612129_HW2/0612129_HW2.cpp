#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <cmath>
#include <windows.h>
#include <ctime>

#define PI 3.14159265358

void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void shaderInit();

int windowSize[2] = { 600, 600 };
int slice = 20;
int stack = 10;
int rotate = 0;
int verticeNumber;

int shadeMode = 0;
int boundMode = 0;

GLuint program;
GLuint vboName;
GLfloat lightPos[3] = { 0.0, 1.0, 0.0 };
GLfloat Ks = 1.0;
GLfloat Kd = 1.0;
GLfloat Ka = 0.5;
GLfloat alpha = 10.6;
GLfloat la[3] = { 0.2, 0.1, 0.1 };
GLfloat ld[3] = { 0.8, 0.4, 0.4 };
GLfloat ls[3] = { 0.8, 0.4, 0.4 };


class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat normal[3];
};
VertexAttribute* drawBall();

int main(int argc, char** argv) {
	//init glut
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");
	//init glew
	glewInit();
	//init shader
	shaderInit();
	//callback function
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	//enter glut loop
	glutMainLoop();

	return 0;
}

void shaderInit() {
	// the func define in shader.h
	GLuint vert = createShader("vert.txt", "vertex");
	GLuint frag = createShader("frag.txt", "fragment");

	program = createProgram(vert, frag);
	////// could need to modify
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute* vertices;
	vertices = drawBall();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void display() {
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.6f,	// eye
		0.0f, 0.0f, 0.0f,		// center
		0.0f, 1.0f, 0.0f);		// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);



	glPushMatrix();
	GLint ShadeModeLoc = glGetUniformLocation(program, "shadeMode");
	glUniform1i(ShadeModeLoc, 4);
	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glBegin(GL_TRIANGLES);
	glEnable(GL_TEXTURE_2D);
	glColor3f(0.5f, 0.4f, 0.0f);
	gluSphere(gluNewQuadric(), 0.05, 30, 30);
	glDisable(GL_TEXTURE_2D);
	glEnd();
	glPopMatrix();

	///////////do something

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	glUseProgram(program);
	
	//input the modelview, rotation matrix matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	///////// self defined
	GLint lightLoc = glGetUniformLocation(program, "LightPosition");
	glUniform3fv(lightLoc, 1, lightPos);
	
	GLint KaLoc = glGetUniformLocation(program, "Ka");
	glUniform1f(KaLoc, Ka);
	
	GLint KdLoc = glGetUniformLocation(program, "Kd");
	glUniform1f(KdLoc, Kd);
	
	GLint KsLoc = glGetUniformLocation(program, "Ks");
	glUniform1f(KsLoc, Ks);
	
	GLint AlphaLoc = glGetUniformLocation(program, "alpha");
	glUniform1f(AlphaLoc, alpha);

	GLint LaLoc = glGetUniformLocation(program, "la");
	glUniform3fv(LaLoc, 1, la);
	
	GLint LdLoc = glGetUniformLocation(program, "ld");
	glUniform3fv(LdLoc, 1, ld);

	GLint LsLoc = glGetUniformLocation(program, "ls");
	glUniform3fv(LsLoc, 1, ls);

	GLint BoundLoc = glGetUniformLocation(program, "BoundMode");
	glUniform1i(BoundLoc, boundMode);

//	GLint ShadeModeLoc = glGetUniformLocation(program, "shadeMode");
	glUniform1i(ShadeModeLoc, shadeMode);


	//sleep
	rotate = (rotate + 1) % 360;
	clock_t goal = 30 + clock();
	while (goal > clock());


	for (int i = 0; i < slice; i++) {
		int sliceVertex = (stack + 1) * 2;
		glDrawArrays(GL_TRIANGLE_STRIP, i * sliceVertex, sliceVertex);
	}


	//state to NULL
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}


VertexAttribute* drawBall() {
	verticeNumber = slice * (stack + 1) * 2;
	int num = 0;
	VertexAttribute* vertex;
	vertex = new VertexAttribute[verticeNumber];
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			vertex[num].position[0] = sin(j * stack_step) * cos(i * slice_step);
			vertex[num].position[1] = cos(j * stack_step);
			vertex[num].position[2] = sin(j * stack_step) * sin(i * slice_step);
			vertex[num].normal[0] = vertex[num].position[0];
			vertex[num].normal[1] = vertex[num].position[1];
			vertex[num].normal[2] = vertex[num].position[2];
			num++;

			vertex[num].position[0] = sin(j * stack_step) * cos((i + 1) * slice_step);
			vertex[num].position[1] = cos(j * stack_step);
			vertex[num].position[2] = sin(j * stack_step) * sin((i + 1) * slice_step);
			vertex[num].normal[0] = vertex[num].position[0];
			vertex[num].normal[1] = vertex[num].position[1];
			vertex[num].normal[2] = vertex[num].position[2];
			num++;
		}
	}
	return vertex;
}


void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1': if (Ks - 0.1 >= 0) Ks -= 0.1; else Ks = 0.0; break;
	case '2': if (Ks + 0.1 <= 1) Ks += 0.1; else Ks = 1.0; break;
	case '3': if (Kd - 0.1 >= 0) Kd -= 0.1; else Kd = 0.0; break;
	case '4': if (Kd + 0.1 <= 1) Kd += 0.1; else Kd = 1.0; break;
	case 'B': 
	case 'b': shadeMode = (shadeMode + 1) % 3; break;
	case 'E': 
	case 'e': boundMode = !boundMode; break;
	default: break;
	};
}

void idle() {
	glutPostRedisplay();
}