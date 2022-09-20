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

int windowSize[2] = { 800, 800 };
int verticeNumber = 20000;
int returnVertexRate = 0;
int kernalNum;
float ballSize = 4;
int cameraTime = 0;

int Mode = 0;



GLuint program;
GLuint vboName;

class Particle {
public:
	GLfloat position[3];
	GLfloat speed[3];
	GLfloat fade;
	GLfloat life;
	int flag;
};
Particle* vertices;
void particleInit(Particle*);
void particleUpdate(Particle*);

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

	vertices = new Particle[verticeNumber];
	particleInit(vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * verticeNumber, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(offsetof(Particle, position)));

/*
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(offsetof(Particle, speed)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(offsetof(Particle, fade)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(offsetof(Particle, life)));
*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void display() {
	if (!cameraTime) {
		clock_t goal = 3000 + clock();
		while (goal > clock());
	}
	//TRY MOVE EARTH INTO SCREEN??????????????????????????????????????????????

	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 50.0f + cameraTime/5,	// eye
		0.0f, 0.0f, 0.0f,		// center
		cos(cameraTime/50.), 1.0f, sin(cameraTime/50.));		// up
	if (cameraTime < 220) cameraTime++;
	//if (cameraTime == 200) Mode = 1;
	if (cameraTime == 215) Mode = 2;
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);


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

	GLint KerLoc = glGetUniformLocation(program, "Kernal");
	glUniform1i(KerLoc, kernalNum);


	glDrawArrays(GL_POINTS, 0, verticeNumber);


	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	Particle* ptr = (Particle*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr)
	{
		particleUpdate(ptr);
		glUnmapBufferARB(GL_ARRAY_BUFFER);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	
	//state to NULL
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}

void particleInit(Particle * v) {
	for (int i = 0; i < verticeNumber; i++) {
//		v[i].position[0] = 0.0;
//		v[i].position[1] = 0.0;
//		v[i].position[2] = 0.0;

		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;

		v[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 70.0;
		v[i].speed[1] = cos(randomj * stepj) * 70.0;
		v[i].speed[2] = sin(randomj * stepj) * sin(randomi * stepi) * 70.0;
		v[i].flag = 0;

		v[i].position[0] = v[i].speed[0] ;
		v[i].position[1] = v[i].speed[1];
		v[i].position[2] = v[i].speed[2];


		v[i].life = 0.5;
		v[i].fade = GLfloat(rand() % 100) / 1000.0 + 0.003;
	}
}

void particleUpdate(Particle* v) {
	if (Mode == 0) {
		kernalNum = 0;
		for (int i = 0; i < verticeNumber; i++) {
			if (v[i].flag > 3)continue;
			v[i].position[0] += v[i].speed[0] / 100.0;
			v[i].position[1] += v[i].speed[1] / 100.0;
			v[i].position[2] += v[i].speed[2] / 100.0;


			v[i].life -= v[i].fade;
			if (v[i].life < 0.0) {
				v[i].life = 1.0;
				v[i].fade = float(rand() % 100) / 1000.0 + 0.003;
				//			v[i].position[0] = 0.0;
				//			v[i].position[1] = 0.0;
				//			v[i].position[2] = 0.0;

				if (i < verticeNumber / 256 * returnVertexRate++ && !v[i].flag) {
					if (returnVertexRate = 254) returnVertexRate--;
					v[i].speed[0] *= -1;
					v[i].speed[1] *= -1;
					v[i].speed[2] *= -1;
					v[i].flag++;

				}

			}
			if (pow(v[i].position[0], 2) + pow(v[i].position[1], 2) + pow(v[i].position[2], 2) < ballSize) {
				kernalNum++;
				if (v[i].flag) {
					v[i].speed[0] *= 0.2 * ballSize / 3;
					v[i].speed[1] *= 0.2 * ballSize / 3;
					v[i].speed[2] *= 0.2 * ballSize / 3;
				}
			}
			if (kernalNum > verticeNumber / 5) ballSize = 3;
			if (kernalNum > verticeNumber / 5 * 3) ballSize = 2;
			if (kernalNum > verticeNumber / 5 * 4) ballSize = 1.5;
		}
	}
	else if (Mode == 1){
		kernalNum -= 2000;
		for (int i = 0; i < verticeNumber; i++) {
			v[i].position[0] *= 1.1;
			v[i].position[1] *= 1.1;
			v[i].position[2] *= 1.1;
			if (i < verticeNumber / 6) {
				v[i].position[0] *= 0.8;
				v[i].position[1] *= 0.8;
				v[i].position[2] *= 0.8;
			}
		}
	}
	else if (Mode == 2){
		for (int i = 0; i < verticeNumber; i++) {
			kernalNum = INT_MIN;
			float stepi = 2 * PI / 180;
			int randomi = rand() % 180;
			float stepj = 2 * PI / 180;
			int randomj = rand() % 180;
			v[i].position[0] = sin(randomj * stepj) * cos(randomi * stepi) * 75.0;
			v[i].position[1] = cos(randomj * stepj) * 75.0;
			v[i].position[2] = sin(randomj * stepj) * sin(randomi * stepi) * 75.0;
		}
	}
	Mode = 3;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {

}

void idle() {
	glutPostRedisplay();
}