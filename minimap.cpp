#include <stdio.h>
#include <stdlib.h>
#include <vector>
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

//constructor
minimap::minimap(int sizeX, int sizeZ){
	this->sizeX = <terrain sizeX scaled>;
	this->sizeZ = <terrain sizeZ scaled>;

	int wallSize = 20;		//size of a 2D wall
	int dotSize = 5;		//size of the player's (camera's) dot.  

}

// Draw the minimap window
void minimap::display_minimap(){
	glutSetWindow(win_minimap);
	glBegin(GL_POINTS);
	//loop will draw all pixels on the map
	for (int x=0; x < sizeX; x++){
		for (int z=0; z < sizeZ; z++){
			if (mazeHeightMap[x][z] != 0){	//if not 0 (wakable path), draw a wall
				glPointSize(wallSize);
				glColor3f(0.0f, 0.0f, 0.0f);
				glVertex2f(x, z);
			}
		}
	}
	glEnd();

	glBegin(GL_POINTS);
	glPointSize(dotSize);
	glColor3f(0.0f,1.0f,0.0f);		//Color of the player dot is green
	glVertex2f(*playerX, *playerZ); //The position of the camera
	glEnd();
	glFlush();
}

/* This goes in the main function.
	//Create heightmap window
	glutInitWindowPosition(700, 50);
	glutInitWindowSize(sizeX,sizeZ);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	win_minimap = glutCreateWindow("Minimap");
	glutDisplayFunc(display_minimap);
	glClearColor(0,0,0,0);  //black by default
	glLoadIdentity();
	gluOrtho2D(0, sizeX, 0, sizeZ);
*/