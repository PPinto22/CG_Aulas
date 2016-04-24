#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#define _PI_ 3.14159

using namespace std;

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

float topo = 1;
float ax = 0;
float ay = 0;
float az = 0;
float xx = 0;
float zz = 0;

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

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 10.0,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	// put the geometric transformations here
	glTranslatef(xx, 0, zz);
	glRotatef(ax, 1, 0, 0);
	glRotatef(ay, 0, 1, 0);
	glRotatef(az, 0, 0, 1);

	// put drawing instructions here
	glBegin(GL_TRIANGLES);
	glColor3d(255, 0, 0);
	glVertex3f(0.0f, topo, 0.0f);
	glVertex3f(-1.0f, 0, 1.0f);
	glVertex3f(1.0f, 0, 1.0f);

	glColor3d(0.0, 255, 0.0);
	glVertex3f(1.0f, 0, -1.0f);
	glVertex3f(0.0f, topo, 0.0f);
	glVertex3f(1.0f, 0, 1.0f);

	glColor3d(0, 0, 255);
	glVertex3d(-1.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, topo, 0.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);

	glColor3d(255, 255, 0);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, topo, 0.0f);
	glVertex3f(-1.0f, 0.0f, -1.0f);

	glColor3d(255, 0, 255);
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	
	glVertex3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);
	glEnd();

	// End of frame
	glutSwapBuffers();
}



// write function to process keyboard events
void keyPressed(unsigned char key, int x, int y) {
	if (key == 'w' || key == 'W') ax -= 10;
	if (key == 's' || key == 'S') ax += 10;
	if (key == 'a' || key == 'A') ay += 10;
	if (key == 'd' || key == 'D') ay -= 10;
	if (key == 'q' || key == 'Q') az += 10;
	if (key == 'e' || key == 'E') az -= 10;
	if (key == 'n' || key == 'N') topo--;
	else if (key == 'm' || key == 'M') topo++;
	glutPostRedisplay();
}

void arrowPressed(int key_code, int x, int y){
	if (key_code == GLUT_KEY_UP) zz--;
	else if (key_code == GLUT_KEY_DOWN) zz++;
	else if (key_code == GLUT_KEY_LEFT) xx--;
	else if (key_code == GLUT_KEY_RIGHT) xx++;
	glutPostRedisplay();
}

// write function to process menu events




int main(int argc, char **argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");


	// Required callback registry 
	glutSpecialFunc(arrowPressed);
	glutKeyboardFunc(keyPressed);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard and menu callbacks



	// put here the definition of the menu 




	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
