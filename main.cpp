#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "terrain.h"

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

int terrainSizeX = 10;
int terrainSizeZ = 10;
terrain mazeTerrain(terrainSizeX, terrainSizeZ, 5);

float upVect[] = {0,1,0};
float camPos[] = {100,100,100};

float light_pos[] = {0, 100, 0,1};
 
float amb1[] = {1, 0, 1, 1};
float diff1[] = {1, 0, 0, 1};
float spec1[] = {1, 1, 0, 1};
 
float m_amb[] = {0.22, 0.11, 0.03, 1.0};
float m_diff[] = {0.78, 0.57, 0.11, 1.0};
float m_spec[] = {0.99, 0.91, 0.81, 1.0};
float shiny = 5;

void drawAxis() {
	// Z-axis = Red
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,0,300);
	glEnd();

	// Y-axis = Green
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,300,0);
	glEnd();

	// X-axis = Blue
	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(300,0,0);
	glEnd();
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, upVect[0],upVect[1],upVect[2]);

	glColor3f(1,1,1); // White light color
	glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff1);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec1);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

	drawAxis();

	glutSwapBuffers();
}

void special(int key, int x, int y) {
	switch(key) {
		
	}

	// Call display function
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		case 'q':
			exit(0);
			break;
	}
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	// Initialize shade model to GL_SMOOTH
	glShadeModel(GL_SMOOTH);

	// Enable lighting
	// glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-2, 2, -2, 2, -2, 2);
	gluPerspective(45, 1, 1, 400);

	//enable backface culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("3D Maze Game");	//creates the window

	// initialize random seed
	srand(time(NULL));

	mazeTerrain.load();

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}