#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "terrain.h"
// Source: http://www.codecolony.de/opengl.htm#Camera2
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

int windowWidth = 600;
int windowHeight = 600;

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

bool moveBack = false; 
bool moveForward = false;

float playerHeight = 2.0;
int camYDirCounter = 0;

/* TEXTURE */
GLubyte *img_data;
int width, height, maximum;

GLubyte* LoadPPM(char* file, int* width, int* height, int* max)
{
	GLubyte* img;
	FILE *fd;
	int n, m;
	int  k, nm;
	char c;
	int i;
	char b[100];
	float s;
	int red, green, blue;
	
	/* first open file and check if it's an ASCII PPM (indicated by P3 at the start) */
	fd = fopen(file, "r");
	fscanf(fd,"%[^\n] ",b);
	if(b[0]!='P'|| b[1] != '3')
	{
		printf("%s is not a PPM file!\n",file); 
		exit(0);
	}
	printf("%s is a PPM file\n", file);
	fscanf(fd, "%c",&c);

	/* next, skip past the comments - any line starting with #*/
	while(c == '#') 
	{
		fscanf(fd, "%[^\n] ", b);
		printf("%s\n",b);
		fscanf(fd, "%c",&c);
	}
	ungetc(c,fd); 

	/* now get the dimensions and max colour value from the image */
	fscanf(fd, "%d %d %d", &n, &m, &k);

	printf("%d rows  %d columns  max value= %d\n",n,m,k);

	/* calculate number of pixels and allocate storage for this */
	nm = n*m;
	img = (GLubyte*)(malloc(3*sizeof(GLuint)*nm));
	s=255.0/k;

	/* for every pixel, grab the read green and blue values, storing them in the image data array */
	for(i=0;i<nm;i++) 
	{
		fscanf(fd,"%d %d %d",&red, &green, &blue );
		img[3*nm-3*i-3]=red*s;
		img[3*nm-3*i-2]=green*s;
		img[3*nm-3*i-1]=blue*s;
	}

	/* finally, set the "return parameters" (width, height, max) and return the image array */
	*width = n;
	*height = m;
	*max = k;

	return img;
}

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

// Source: http://guidedhacking.com/showthread.php?6588-OpenGL-Draw-a-crosshair
void drawCrosshair() {
    glPushMatrix();
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3ub(240, 240, 240);//white
    glLineWidth(2.0);
    glBegin(GL_LINES);
    //horizontal line
    glVertex2i(windowWidth / 2 - 7, windowHeight / 2);
    glVertex2i(windowWidth / 2 + 7, windowHeight / 2);
    glEnd();
    //vertical line
    glBegin(GL_LINES);
    glVertex2i(windowWidth / 2, windowHeight / 2 + 7);
    glVertex2i(windowWidth / 2, windowHeight / 2 - 7);
    glEnd();

    glPopMatrix();

    //Set a new projection matrix
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
    gluPerspective(40.0,(GLdouble)windowWidth/(GLdouble)windowHeight,0.5,400.0);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Camera.Render();

	// gluLookAt(	Camera.Position.x,Camera.Position.y,Camera.Position.z,
	// 			Camera.ViewDir.x + Camera.Position.x,Camera.ViewDir.y + Camera.Position.y,Camera.ViewDir.z + Camera.Position.z,
	// 			Camera.UpVector.x,Camera.UpVector.y,Camera.UpVector.z);

	// Camera.showPosition();
	// Camera.showViewPoint();

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

	// glPushMatrix();
	
	//do the rotation - rotate about the Y axis by angle ang
	// glRotatef(ang, 0, 1, 0);
	
	//draw the teapot
	
	/* MATERIALS */
	
	
	glColor3f(1,0,0);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	
	//pop the matrix back to what it was prior to the rotation
	// glPopMatrix();

	mazeTerrain.draw();

	drawCrosshair();

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
			// "Pretend" to move forward
			Camera.MoveForward( -1 );

			// Restrict movement along the y-axis (i.e can't fly or go through the ground)
			if (camYDirCounter != 0) {
				printf("PosY:%f PlayerHeight:%f ViewY:%f\n", Camera.Position.y, playerHeight, abs(Camera.ViewDir.y));
				Camera.MoveForward(1);		// reset pretend move
				return;
			}

			// If the "pretend" move doesn't cause a collision, actually move forward
			// otherwise, reset the "pretend" move
			if (!mazeTerrain.checkCollision(Camera.Position.x, Camera.Position.z) || moveForward == true) {
				Camera.MoveForward( 1 );	// reset pretend move
				Camera.MoveForward( -0.5 );	// actually move
				moveForward = false;
				moveBack = false;
			} 
			else {
				Camera.MoveForward( 1 );	// reset pretend move
				moveBack = true;
				printf("Move Back\n");
			}
			break;
		case GLUT_KEY_DOWN:
			// "Pretend" to move backward
			Camera.MoveForward( 1 );

			// Restrict movement along the y-axis (i.e can't fly or go through the ground)
			if (camYDirCounter != 0) {
				printf("PosY:%f PlayerHeight:%f ViewY:%f\n", Camera.Position.y, playerHeight, abs(Camera.ViewDir.y));
				Camera.MoveForward(1);		// reset pretend move
				return;
			}

			if (!mazeTerrain.checkCollision(Camera.Position.x, Camera.Position.z) || moveBack == true) {
				Camera.MoveForward( -1 );	// reset pretend move
				Camera.MoveForward( 0.5 );	// actually move
				moveForward = false;
				moveBack = false;
			} 
			else {
				Camera.MoveForward( -1 );	// reset pretend move
				moveForward = true;
				printf("Move Forward\n");
			}
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
			camYDirCounter++;
			printf("ViewY:%f\n", Camera.ViewDir.y*1000000);
			display();
			break;
		case 's':		
			// Camera.RotateX(-5.0);
			Camera.ViewDir.y -= 0.1;
			camYDirCounter--;
			printf("ViewY:%f\n", Camera.ViewDir.y*1000000);
			display();
			break;
	}
}

void init(void) {
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

void reshape(int x, int y) {
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

	Camera.Move( F3dVector(-31.0, playerHeight, 35.0 ));
	Camera.MoveForward( 1.0 );

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

    /* TEXTURES */
	glEnable(GL_TEXTURE_2D);

	img_data = LoadPPM("marble.ppm", &width, &height, &maximum);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, img_data); 

    // image = LoadPPM("interface.ppm", &width, &height, &max);

	glEnable(GL_DEPTH_TEST);
	init();

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}