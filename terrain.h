#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
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
    SF3dVector * endPos;

	terrain(int sizeX, int sizeZ, int height);
	void load(CCamera * cam);
	void draw();
	bool checkCollision(float xPos, float zPos);
    bool reachedEnd(CCamera *cam);

private:
	vector<vertex3D>*verts;
	vector<faces3D>*faces;
	vector<vertex3D>*faceNormals;
	vector<vertex3D>*vertexNormals;

	void generateMaze(CCamera *cam, int * startX, int * startZ, int * endX, int *endZ);
    float* convertHeightMapToFace(int i, int j);
    void convertHeightMapToFace2(int i, int j, float *posX, float *posZ);
	void showMaze();
	void calcFaceNormals();
	void calcVertexNormals();
	void drawWall(int v1, int v2, int n);
    void startDFS(int x, int z);;
};
#endif
