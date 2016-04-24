#include <stdlib.h>
#include <vector>
#include <glew.h>
#include <GL/glut.h>
#include <math.h>
#pragma comment(lib,"glew32.lib")

#define _PI_ 3.14159
#define N 1

float alpha = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

// declare variables for VBO id 
float *vertexB; int verticesB;
GLuint buffers[N];

class Ponto {
private: float xval, yval, zval;

public:
	Ponto(float x, float y, float z) {
		xval = x;
		yval = y;
		zval = z;
	}
	float x() { return xval; }
	float y() { return yval; }
	float z() { return zval; }

};

void sphericalToCartesian() {

	camX = radius * cos(beta) * sin(alpha);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alpha);
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


/*-----------------------------------------------------------------------------------
Drawing cylinder with VBOs
-----------------------------------------------------------------------------------*/

void drawCylinder() {

	//	Bind and semantics
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	//  Draw
	glDrawArrays(GL_TRIANGLES, 0, verticesB);

}

/*-----------------------------------------------------------------------------------
Create the VBO for the cylinder
-----------------------------------------------------------------------------------*/

void bufferAdd(float* buffer, Ponto p, int indice) {
	buffer[indice] = p.x();
	buffer[indice + 1] = p.y();
	buffer[indice + 2] = p.z();
}

void prepareCylinder(float altura, float radius, int lados) {

	// Enable buffer ??

	// Allocate and fill vertex array
	verticesB = lados * 6 + 2*lados * 3;
	vertexB = (float*)malloc(3*verticesB*sizeof(float));

	int indice = 0;
	float alpha = 0;
	float x, y, z;
	std::vector<Ponto> pontos;
	Ponto sup(0, altura / 2, 0);
	Ponto inf(0, -altura / 2, 0);

	for (int i = 0; i < lados; i++) {
		float alpha = i*(2 * _PI_ / lados);
		z = radius*cos(alpha);
		x = radius*sin(alpha);
		y = altura / 2;
		Ponto p(x, y, z);
		pontos.push_back(p);
	}
	for (int i = 0; i < lados; i++) {
		int i_dir = (i + 1) % lados;
		Ponto esqInf = pontos[i];
		Ponto dirInf = pontos[i_dir];
		Ponto esqSup = Ponto(pontos[i].x(), -pontos[i].y(), pontos[i].z());
		Ponto dirSup = Ponto(pontos[i_dir].x(), -pontos[i_dir].y(), pontos[i_dir].z());

		bufferAdd(vertexB, esqInf, indice); indice += 3;
		bufferAdd(vertexB, dirInf, indice); indice += 3;
		bufferAdd(vertexB, dirSup, indice); indice += 3;

		bufferAdd(vertexB, esqInf, indice); indice += 3;
		bufferAdd(vertexB, dirSup, indice); indice += 3;
		bufferAdd(vertexB, esqSup, indice); indice += 3;

		bufferAdd(vertexB, dirSup, indice); indice += 3;
		bufferAdd(vertexB, sup, indice); indice += 3;
		bufferAdd(vertexB, esqSup, indice); indice += 3;

		bufferAdd(vertexB, esqInf, indice); indice += 3;
		bufferAdd(vertexB, inf, indice); indice += 3;
		bufferAdd(vertexB, dirInf, indice); indice += 3;
	}

	// Generate VBOs
	glGenBuffers(N, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, verticesB*3*sizeof(float), vertexB, GL_STATIC_DRAW);
	free(vertexB);
}

/*-----------------------------------------------------------------------------------
RENDER SCENE
-----------------------------------------------------------------------------------*/

int frame = 0;
int time = glutGet(GLUT_ELAPSED_TIME);
int timebase = time;
float fps;

void renderScene(void) {

	float pos[4] = { 1.0, 1.0, 1.0, 0.0 };

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	drawCylinder();

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
	}

	char* fps_string = (char*)malloc(8 * sizeof(char));
	sprintf(fps_string, "%.0f fps", fps);
	glutSetWindowTitle(fps_string);

	// End of frame
	glutSwapBuffers();
}


// special keys processing function
void processKeys(int key, int xx, int yy)
{
	switch (key) {

	case GLUT_KEY_RIGHT:
		alpha -= 0.1; break;

	case GLUT_KEY_LEFT:
		alpha += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_UP: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius += 0.1f; break;

	}
	sphericalToCartesian();
	glutPostRedisplay();
}


void main(int argc, char **argv) {

	// initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("CG@DI-UM");

	// callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutSpecialFunc(processKeys);

	glewInit();

	// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// init
	sphericalToCartesian();
	glEnableClientState(GL_VERTEX_ARRAY);
	prepareCylinder(2, 1, 1000);

	// enter GLUTs main cycle
	glutMainLoop();
}
