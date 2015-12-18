#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <vector>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif


#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
#include <iostream>
#include <math.h>
#include "terrain.h"






using namespace std;

struct vertex3D {
	float x, y, z;
	int faces[4] = {-1,-1,-1,-1};
	int numFaces = 0;

	void set(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void addFace(int f) {
		for (int i = 0; i < 4; ++i) {
			if (faces[i] == -1) {
				faces[i] = f;
				numFaces++;
				break;
			}
		}
	}

	void show() {
		printf("%f,%f,%f\n", x, y, z);
	}

	void showFaces() {
		printf("Faces: %d,%d,%d,%d\n", faces[0], faces[1], faces[2], faces[3]);
	}
};

struct faces3D {
	vertex3D v1, v2, v3, v4;

	void set(vertex3D v1, vertex3D v2, vertex3D v3, vertex3D v4) {
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		this->v4 = v4;
	}
};

class terrain {
public:
	int sizeX, sizeZ, height;
	int **mazeHeightMap;

	terrain(int sizeX, int sizeZ, int height);
	void load();
	void draw();
	bool checkCollision(float xPos, float zPos);

private:
	vector<vertex3D>*verts;
	vector<faces3D>*faces;
	vector<vertex3D>*faceNormals;
	vector<vertex3D>*vertexNormals;

	void generateMaze(int halfWidth, int halfLength);
	void showMaze();
	void calcFaceNormals();
	void calcVertexNormals();
	void drawWall(int v1, int v2);
    void startDFS(int x, int z);;

};
#endif
