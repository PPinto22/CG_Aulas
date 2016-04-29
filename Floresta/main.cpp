#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>

#pragma comment(lib, "devil.lib")
#pragma comment(lib,"glew32.lib")


#define _USE_MATH_DEFINES
#include <math.h>

#define _PI_ 3.14159
#define ARVORES 500
#define RAIO_TRONCO 2
#define RAIO_ARVORE 4
#define ALTURA_TRONCO 5
#define ALTURA_ARVORE 10
#define R 50
#define RI 35
#define RC 15
#define VERMELHOS 20
#define AZUIS 8

float alpha = 0.0f, beta = 0.75f, radius = 400.0f;
float camX, camY, camZ;
float alphaAzuis = 0.0f, alphaVermelhos = 0.0f;
int draw_mode = 0; //0 = Fill, 1 = Line, 2 = Point

unsigned int t, tw, th;
unsigned char *imageData;

GLuint* buffers;

void sphericalToCartesian() {

	camX = radius * cos(beta) * sin(alpha);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alpha);
}

float h(int i, int j) {
	return (float)imageData[i*tw+j]/255.0f*50.0f;
}

float hf(float x, float z) {
	int x1 = (int)floor(x);
	int x2 = x1 + 1;
	int z1 = (int)floor(z);
	int z2 = z1 + 1;
	float fz = z - z1;
	float fx = x - x1;
	float h_x1_z = h(x1, z1)*(1 - fz) + h(x1, z2)*fz;
	float h_x2_z = h(x2, z1)*(1 - fz) + h(x2, z2)*fz;
	float height_xz = h_x1_z*(1 - fx) + h_x2_z*fx;
	return height_xz;
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	// Drawing Mode
	switch (draw_mode) {
	case 0:
		glPolygonMode(GL_FRONT, GL_FILL);
		break;
	case 1:
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT, GL_POINT);
		break;
	}

	// Plano
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColor3d(1, 1, 1);
	for (int i = 0; i < th - 1; i++) {
		int inicio = i * 2 * th;
		int count = 2 * th;
		glDrawArrays(GL_TRIANGLE_STRIP, inicio, count);
	}

	//�rvores
	srand(314);
	int arvore = 0;
	while(arvore<ARVORES){
		float x = ((float)rand() / RAND_MAX)*(tw-5)-((tw-5)/2);
		float z = ((float)rand() / RAND_MAX)*(th-5)-((th-5)/2);

		if (pow(x, 2) + pow(z, 2) >= pow(R, 2)) {
			float y = hf(x, z);
			//Tronco
			glPushMatrix();
			glTranslatef(x, y, z);
			glRotatef(-90, 1, 0, 0);
			glColor3d(140/255.0f, 70/255.0f, 20/255.0f);
			glutSolidCone(RAIO_TRONCO, ALTURA_TRONCO, 10, 10);
			glPopMatrix();
			//Arvore
			glPushMatrix();
			glTranslatef(x, y+ALTURA_TRONCO-2, z);
			glRotatef(-90, 1, 0, 0);
			glColor3d(0, 153/255.0f, 0);
			glutSolidCone(RAIO_ARVORE, ALTURA_ARVORE, 10, 10);
			glPopMatrix();
			arvore++;
		}
	}

	//Vermelhos
	glColor3d(1, 0, 0);
	float incrementosV = M_PI*2 / VERMELHOS;
	for (int i = 0; i < VERMELHOS; i++) {
		alphaVermelhos += incrementosV;
		float x = RI*sin(alphaVermelhos);
		float z = RI*cos(alphaVermelhos);
		glPushMatrix();
		glTranslated(x, 2.0f, z);
		glutSolidTeapot(2);
		glPopMatrix();
	}
	alphaVermelhos += 0.01;

	// End of frame
	glutSwapBuffers();
}

void loadTerrain() {
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage((ILstring)"terreno2.jpg");
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	imageData = ilGetData();
	buffers = (GLuint*)malloc(1 * sizeof(GLuint));
	float* vertexB = (float*)malloc(sizeof(float)*(th-1)*tw*6);
	int dx = th / 2;
	int dz = tw / 2;
	int p = 0;
	for (int z = 0; z < th-1; z++) {
		for (int x = 0; x < tw; x++) {
			vertexB[p++] = x - dx;
			vertexB[p++] = h(z,x);
			vertexB[p++] = z - dz;

			vertexB[p++] = x - dx;
			vertexB[p++] = h(z+1,x);
			vertexB[p++] = z + 1 - dz;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*(th-1)*tw*6, vertexB, GL_STATIC_DRAW);
}

void menuHandler(int id_op) {
	switch (id_op) {
	case 1:
		draw_mode = 0;
		break;
	case 2:
		draw_mode = 1;
		break;
	case 3:
		draw_mode = 2;
		break;
	}
	glutPostRedisplay();
}

void processKeys(int key, int xx, int yy)
{
	switch (key) {

	case GLUT_KEY_RIGHT:
		alpha -= 0.05; break;

	case GLUT_KEY_LEFT:
		alpha += 0.05; break;

	case GLUT_KEY_UP:
		beta += 0.03f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.03f;
		if (beta < 0.0f)
			beta = 0.0f;
		break;

	case GLUT_KEY_PAGE_UP: radius -= 10.0f;
		if (radius < 10.0f)
			radius = 10.0f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius += 10.0f; break;

	}
	sphericalToCartesian();
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	// put init here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI");

	glewInit();

	// put callback registration here
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutSpecialFunc(processKeys);
	glEnableClientState(GL_VERTEX_ARRAY);

	//Menu definitions
	glutCreateMenu(menuHandler);
	glutAddMenuEntry("Fill", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Point", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	ilInit();

	loadTerrain();

	// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// enter GLUT's main loop
	sphericalToCartesian();
	glutMainLoop();

	return 1;
}