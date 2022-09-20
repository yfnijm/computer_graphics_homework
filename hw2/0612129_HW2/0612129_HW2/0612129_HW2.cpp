#include "FreeImage.h"
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
void textureInit();

int windowSize[2] = { 600, 600 };
int slice = 360;
int stack = 180;
int rotate = 0;
int verticeNumber;
GLuint program;
GLuint vboName;
GLuint texture;


class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat texcoord[2];
};
VertexAttribute* drawEarth();

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
	textureInit();
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
	vertices = drawEarth();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void textureInit() {
	glEnable(GL_TEXTURE_2D);
	char filename[] = "earth_texture_map.jpg";
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
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
	gluLookAt(0.0f, 0.0f, 3.0f,	// eye
		0.0f, 0.0f, 0.0f,		// center
		0.0f, 1.0f, 0.0f);		// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

///////////do something

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "Texture");

	glUseProgram(program);

	//input the modelview, rotation matrix matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	/////////
	GLint numberIndex = glGetUniformLocation(program, "number");
	GLfloat x_offset = rotate / 360.0;
	glUniform2f(numberIndex, x_offset, 0);
	rotate = (rotate + 1) % 360;
	clock_t goal = 30 + clock();
	while (goal > clock());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	for (int i = 0; i < slice; i++) {
		int sliceVertex = (stack + 1) * 2;
		glDrawArrays(GL_TRIANGLE_STRIP, i * sliceVertex, sliceVertex);
	}

	
	//state to NULL
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}


VertexAttribute* drawEarth() {
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
			vertex[num].texcoord[0] = 1.0 / slice * i;
			vertex[num].texcoord[1] = cos(j * stack_step) / 2 + 0.5;
			num++;

			vertex[num].position[0] = sin(j * stack_step) * cos((i + 1) * slice_step);
			vertex[num].position[1] = cos(j * stack_step);
			vertex[num].position[2] = sin(j * stack_step) * sin((i + 1) * slice_step);
			vertex[num].texcoord[0] = 1.0 / slice * (i + 1);
			vertex[num].texcoord[1] = cos(j * stack_step) / 2 + 0.5;
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

}

void idle() {
	glutPostRedisplay();
}