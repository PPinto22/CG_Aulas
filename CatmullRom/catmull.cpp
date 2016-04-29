#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>

float camX = 0, camY, camZ = 5;
int startX, startY, tracking = 0;

int alpha = 0, beta = 0, r = 5;

#define POINT_COUNT 5
#define LAP_TIME 10000 //milisegundos
// Points that make up the loop for catmull-rom interpolation
float p[POINT_COUNT][3] = { { -1,-1,0 },{ -1,1,0 },{ 1,1,0 },{ 0,0,0 },{ 1,-1,0 } };

void cross(float *a, float *b, float *res) {
	res[0] = a[1] * b[2] - a[2] * b[1]; res[1] = a[2] * b[0] - a[0] * b[2]; res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float *a) {
	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]); 
	a[0] = a[0] / l; 
	a[1] = a[1] / l; 
	a[2] = a[2] / l;
}

void buildRotMatrix(float *x, float *y, float *z, float *m) {
	m[0] = x[0];
	m[1] = x[1];
	m[2] = x[2];
	m[3] = 0;
	m[4] = y[0];
	m[5] = y[1];
	m[6] = y[2];
	m[7] = 0;
	m[8] = z[0]; 
	m[9] = z[1];
	m[10] = z[2];
	m[11] = 0;
	m[12] = 0; 
	m[13] = 0; 
	m[14] = 0; 
	m[15] = 1;
}

void getCatmullRomPoint(float t, int *indices, float *res, float* dir) {

	// catmull-rom matrix
	float M[4][4] = { { -0.5f,  1.5f, -1.5f,  0.5f },
	{ 1.0f, -2.5f,  2.0f, -0.5f },
	{ -0.5f,  0.0f,  0.5f,  0.0f },
	{ 0.0f,  1.0f,  0.0f,  0.0f } };

	float T[1][4] = { { pow(t,3),pow(t,2),t,1.0f } };
	float Td[1][4] = { { 3 * pow(t,2), 2 * t, 1, 0} };

	res[0] = 0.0; res[1] = 0.0; res[2] = 0.0;
	dir[0] = 0.0; dir[1] = 0.0; dir[2] = 0.0;

	float P[4][3];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			P[i][j] = p[indices[i]][j];
		}
	}

	//Calcular T*M e Td*M
	float TM[1][4] = { {0.0,0.0,0.0,0.0} };
	float TdM[1][4] = { { 0.0,0.0,0.0,0.0 } };
	for (int j = 0; j < 4; j++) {
		for (int k = 0; k < 4; k++) {
			TM[0][j] += T[0][k] * M[k][j];
			TdM[0][j] += Td[0][k] * M[k][j];
		}
	}
	
	//Calcular T*M*P e Td*M*P
	for (int xyz = 0; xyz < 3; xyz++) {
		for (int i = 0; i < 4; i++) {
			res[xyz] += TM[0][i] * P[i][xyz];
			dir[xyz] += TdM[0][i] * P[i][xyz];
		}
	}
}


// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *res, float* dir) {

	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

				   // indices store the points
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;

	getCatmullRomPoint(t, indices, res, dir);
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45, ratio, 1, 1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderCatmullRomCurve() {
	// desenhar a curva usando segmentos de reta - GL_LINE_LOOP
	float res[3];
	float dir[3];
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 1000*3; i++) {
		getGlobalCatmullRomPoint(i / 1000.0f, res, dir);
		glVertex3f(res[0], res[1], res[2]);
	}
	glEnd();
}

int time = 0;
float up[3] = { 0.0f, 1.0f, 0.0f };

void renderScene(void) {

	static float t = 0;
	float res[3], dir[3];
	float left[3];
	float x[3], y[3], z[3];
	float m[16];

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	renderCatmullRomCurve();

	time = glutGet(GLUT_ELAPSED_TIME) % LAP_TIME;
	t = (float)time / LAP_TIME;

	getGlobalCatmullRomPoint(t, res, dir); normalize(dir);
	cross(up, dir, left); normalize(left);
	cross(dir, left, up); normalize(up);

	buildRotMatrix(dir, up, left, m);

	glTranslatef(res[0], res[1], res[2]);
	glMultMatrixf(m);

	glutSolidTeapot(0.1);

	glutSwapBuffers();
}


void processMouseButtons(int button, int state, int xx, int yy)
{
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy)
{
	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {

		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux *							     sin(betaAux * 3.14 / 180.0);
}


void main(int argc, char **argv) {

	// inicialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("CG@DI-UM");

	// callback registration 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// mouse callbacks
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle 
	glutMainLoop();
}