#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "terrain.h"
#include "camera.h"

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

#define ShowUpvector

CCamera Camera;

int terrainSizeX = 10;
int terrainSizeZ = 10;
terrain mazeTerrain(terrainSizeX, terrainSizeZ, 5);

float upVect[] = {0,1,0};
float camPos[] = {50,0,50};
float camFocus[] = {0,0,0};

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

	Camera.Render();

	gluLookAt(	Camera.Position.x,Camera.Position.y,Camera.Position.z,
				Camera.ViewDir.x + Camera.Position.x,Camera.ViewDir.y + Camera.Position.y,Camera.ViewDir.z + Camera.Position.z,
				Camera.UpVector.x,Camera.UpVector.y,Camera.UpVector.z);

	Camera.showPosition();
	Camera.showViewPoint();

	// gluLookAt(camPos[0], camPos[1], camPos[2], camFocus[0],camFocus[1],camFocus[2], upVect[0],upVect[1],upVect[2]);

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
	mazeTerrain.draw();

	glFlush();
	glutSwapBuffers();
}

void special(int key, int x, int y) {
	// int fx = camFocus[0];
	// int fz = camFocus[2];
	// int px = camPos[0];
	// int pz = camPos[2];
	// float slope = (fz - pz) / (fx - px);
	// float b = fz - (slope*fx);

	switch(key) {
		case GLUT_KEY_UP:
			Camera.MoveForward( -0.1 );
			display();
			break;
		case GLUT_KEY_DOWN:
			Camera.MoveForward( 0.1 ) ;
			break;
		case GLUT_KEY_RIGHT:
			Camera.RotateY(-5.0);
			break;
		case GLUT_KEY_LEFT:
			Camera.RotateY(5.0);
			break;
	}

	// Call display function
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		case 'q':
			exit(0);
			break;
		case 'w':		
			// Camera.RotateX(5.0);
			Camera.ViewDir.y += 0.1;
			display();
			break;
		case 's':		
			// Camera.RotateX(-5.0);
			Camera.ViewDir.y -= 0.1;
			display();
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
	// gluPerspective(45, 1, 1, 400);

	//enable backface culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;  //Nothing is visible then, so return
	
	//Set a new projection matrix
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	//Angle of view:40 degrees
	//Near clipping plane distance: 0.5
	//Far clipping plane distance: 20.0
	gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,400.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,x,y);  //Use the whole window for rendering
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

	Camera.Move( F3dVector(-4.0, 1.0, 11.0 ));
	Camera.MoveForward( 1.0 );

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}