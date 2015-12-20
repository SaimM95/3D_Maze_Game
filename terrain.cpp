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

using namespace std;

// Constructor
terrain::terrain(int sizeX, int sizeZ, int height) {
	this->sizeX = sizeX;
	this->sizeZ = sizeZ;
	this->height = height;

	verts = new vector<vertex3D>((sizeX+1)*(sizeZ+1));
	faces = new vector<faces3D>(sizeX*sizeZ);
	faceNormals = new vector<vertex3D>(faces->size());
	vertexNormals = new vector<vertex3D>(verts->size());

	// mazeHeightMap is a 2d representation of the maze where each element's value is either 0 (walkable path) 
	// or height (wall)
	// NOTE THE ORDER OF sizeX,sizeZ (i.e. (x,z)) in array - positive x-axis points DOWN, positive z-axis points LEFT
	mazeHeightMap = new int*[sizeX];
	for (int x = 0; x < sizeX; ++x) {
		mazeHeightMap[x] = new int[sizeZ];
	}

	for (int x = 0; x < sizeX; ++x) {
		for (int z = 0; z < sizeZ; ++z) {
			mazeHeightMap[x][z] = 0;
		}
	}
}

// Load the maze terrain
void terrain::load() {
	generateMaze();
	showMaze();

	// Use maze2d.mazeHeightMap[][] for height (y-coords) of each vertex
	// ...
	// Populate verts, faces, faceNormals and vertexNormals arrays
	// ...

	// Set x, y, z values for all the vertices in such a way that it translates into a flat plane of points
	// i.e. all points have height 0
	int vertexCount = 0;
	for (int x = (-sizeX/2)*10; x <= (sizeX/2)*10; x+=10) {
		for (int z = (sizeZ/2)*10; z >= (-sizeZ/2)*10; z-=10) {
			verts->at(vertexCount).set(x,0,z);
			vertexCount++;
		}
	}

	// for (int x = 0; x <= sizeX; ++x) {
	// 	for (int z = 0; z <= sizeZ; ++z) {
	// 		printf("heightX:%d heightZ:%d Height:%f\n", x, z, mazeHeightMap[x][z]);
	// 	}
	// }

	// Fill the faces vector with vertices in a sequential manner 
	// (i.e. each 4 vertices that should appear together, correspond to a single polygon)
	int xCount = 0;
	int heightX = 0; 
	int heightZ = 0;
	for (int f = 0; f < faces->size(); ++f) {
		if (f > 0 && f%sizeX == 0) xCount++;
		int v1 = f+sizeZ+1+xCount;
		int v2 = f+sizeZ+2+xCount;
		int v3 = f+1+xCount;
		int v4 = f+0+xCount;

		faces->at(f).set(verts->at(v1),verts->at(v2),verts->at(v3),verts->at(v4));

		verts->at(v1).addFace(f);
		verts->at(v2).addFace(f);
		verts->at(v3).addFace(f);
		verts->at(v4).addFace(f);

		// printf("heightX:%d heightZ:%d Height:%d\n", heightX, heightZ, mazeHeightMap[heightX][heightZ]);

		// verts->at(v1).y = mazeHeightMap[heightX][heightZ];
		// verts->at(v2).y = mazeHeightMap[heightX][heightZ];
		// verts->at(v3).y = mazeHeightMap[heightX][heightZ];
		// verts->at(v4).y = mazeHeightMap[heightX][heightZ];

		heightZ++;
		if (heightZ >= sizeZ) {
			heightX++;
			heightZ = 0;
		}
	}

	calcFaceNormals();
	calcVertexNormals();

	// Testing convert function
	// float *centerPoint = convertHeightMapToFace(1,1);
	// printf("Center Point: %f,%f,%f\n", centerPoint[0], centerPoint[1], centerPoint[2]);
}

// Draw the terrain using glut functions
void terrain::draw() {
	int xCount = 0;
	int heightX = 0; 
	int heightZ = 0;

	int extrudeVal = 0;

	// use quads or triangles to draw the things in your picture
	for (int i = 0; i < faces->size(); ++i) {
		if (i > 0 && i%sizeX == 0) xCount++;
		int v1 = i+sizeZ+1+xCount;
		int v2 = i+sizeZ+2+xCount;
		int v3 = i+1+xCount;
		int v4 = i+0+xCount;

		if (mazeHeightMap[heightX][heightZ] == height) {
			extrudeVal = height;

			drawWall(v1,v2);
			drawWall(v2,v3);
			drawWall(v3,v4);
			drawWall(v4,v1);

			
		}
		else {
			extrudeVal = 0;
		}

		glColor3f(0.8,0.8,0.8);
	
		glBegin(GL_POLYGON);
			glNormal3f(vertexNormals->at(v1).x, vertexNormals->at(v1).y, vertexNormals->at(v1).z);
			glVertex3f(faces->at(i).v1.x, faces->at(i).v1.y + extrudeVal, faces->at(i).v1.z);

			glNormal3f(vertexNormals->at(v2).x, vertexNormals->at(v2).y, vertexNormals->at(v2).z);
			glVertex3f(faces->at(i).v2.x, faces->at(i).v2.y + extrudeVal, faces->at(i).v2.z);

			glNormal3f(vertexNormals->at(v3).x, vertexNormals->at(v3).y, vertexNormals->at(v3).z);
			glVertex3f(faces->at(i).v3.x, faces->at(i).v3.y + extrudeVal, faces->at(i).v3.z);

			glNormal3f(vertexNormals->at(v4).x, vertexNormals->at(v4).y, vertexNormals->at(v4).z);
			glVertex3f(faces->at(i).v4.x, faces->at(i).v4.y + extrudeVal, faces->at(i).v4.z);
		glEnd();

		heightZ++;
		if (heightZ >= sizeZ) {
			heightX++;
			heightZ = 0;
		}
	}
}

bool terrain::checkCollision(float xPos, float zPos) {
	int heightX = 0; 
	int heightZ = 0;

	for (int i = 0; i < faces->size(); ++i) {
		if (xPos >= faces->at(i).v3.x && 
			xPos <= faces->at(i).v1.x && 
			zPos >= faces->at(i).v3.z && 
			zPos <= faces->at(i).v1.z) {
			// printf("FaceXLow:%f FaceXHigh:%f FaceZLow:%f FaceZHigh:%f X:%d Z:%d -- On face:%d\n", 
			// 	faces->at(i).v3.x, faces->at(i).v1.x, faces->at(i).v3.z, faces->at(i).v1.z, xPos, zPos, i);
			if (mazeHeightMap[heightX][heightZ] < height) return false;
		}

		heightZ++;
		if (heightZ >= sizeZ) {
			heightX++;
			heightZ = 0;
		}
	// 	// else printf("COLLISSION! FaceXLow:%f FaceXHigh:%f FaceZLow:%f FaceZHigh:%f X:%d Z:%d \n", 
	// 	// 	faces->at(i).v3.x, faces->at(i).v1.x, faces->at(i).v1.z, faces->at(i).v3.z, xPos, zPos);
	}

	printf("COLLISSION!\n");
	return true;

	// for (int i = 0; i < verts->size(); ++i) {
	// 	printf("(%d,%d) ", int(verts->at(i).x), int(verts->at(i).z));
	// 	if ((i+1)%11 == 0) printf("\n");
	// }

	// printf("COLLISSION! FaceXLow:%f FaceXHigh:%f FaceZLow:%f FaceZHigh:%f X:%d Z:%d \n", 
	// 		faces->at(10).v3.x, faces->at(10).v1.x, faces->at(10).v1.z, faces->at(10).v3.z, xPos, zPos);
}

// Private function: Populate mazeHeightMap array
void terrain::generateMaze() {
	// Put walls on all the edges of the terrain
	for (int x = 0; x < sizeX; ++x) {
		for (int z = 0; z < sizeZ; ++z) {
			if (x == 0 || z == 0 || x == sizeX-1 || z == sizeZ-1)
				mazeHeightMap[x][z] = height;
		}
	}

	// mazeHeightMap[10][10] = {
	// 				{5,5,5,5,5,5,5,5,5,5},
	// 			s->	{0,0,5,0,5,0,0,0,0,5},
	// 				{5,0,5,0,5,0,5,0,5,5},
	// 				{5,0,5,0,5,0,5,0,5,5},
	// 				{5,0,5,0,5,0,0,0,0,5},
	// 				{5,0,5,0,5,0,5,5,0,5},
	// 				{5,0,0,0,5,0,5,0,0,5},
	// 				{5,0,5,0,5,0,5,0,5,5},
	// 				{5,0,5,0,0,0,5,0,0,0}, <- f
	// 				{5,5,5,5,5,5,5,5,5,5}
	// 			};

	// Generate maze walls
	// ...
	mazeHeightMap[1][0] = 0;
	mazeHeightMap[1][2] = 5;
	mazeHeightMap[2][2] = 5;
	mazeHeightMap[3][2] = 5;
	mazeHeightMap[4][2] = 5;
	mazeHeightMap[5][2] = 5;
	mazeHeightMap[7][2] = 5;
	mazeHeightMap[8][2] = 5;
	mazeHeightMap[1][4] = 5;
	mazeHeightMap[2][4] = 5;
	mazeHeightMap[3][4] = 5;
	mazeHeightMap[4][4] = 5;
	mazeHeightMap[5][4] = 5;
	mazeHeightMap[6][4] = 5;
	mazeHeightMap[7][4] = 5;
	mazeHeightMap[2][6] = 5;
	mazeHeightMap[3][6] = 5;
	mazeHeightMap[5][6] = 5;
	mazeHeightMap[6][6] = 5;
	mazeHeightMap[7][6] = 5;
	mazeHeightMap[8][6] = 5;
	mazeHeightMap[5][7] = 5;
	mazeHeightMap[2][8] = 5;
	mazeHeightMap[3][8] = 5;
	mazeHeightMap[7][8] = 5;
	mazeHeightMap[8][9] = 0;
}

// Private function: Print out the mazeHeightMap array
void terrain::showMaze() {
	for (int x = 0; x < sizeX; ++x) {
		for (int z = 0; z < sizeZ; ++z) {
			printf("%d ", mazeHeightMap[x][z]);
		}
		printf("\n");
	}
}

// Private function: Calculate and populate the faceNormals
void terrain::calcFaceNormals() {
	float normalLength;
	for (int i = 0; i < faceNormals->size(); ++i) {
		float v1[] = {faces->at(i).v2.x - faces->at(i).v1.x, faces->at(i).v2.y - faces->at(i).v1.y, 
					faces->at(i).v2.z - faces->at(i).v1.z};

		float v2[] = {faces->at(i).v4.x - faces->at(i).v1.x, faces->at(i).v4.y - faces->at(i).v1.y, 
			faces->at(i).v4.z - faces->at(i).v1.z};

		// Cross Product
		faceNormals->at(i).x = v1[1]*v2[2] - v1[2]*v2[1];
		faceNormals->at(i).y = -1 * (v1[0]*v2[2] - v1[2]*v2[0]);
		faceNormals->at(i).z = v1[0]*v2[1] - v1[1]*v2[0];

		// Normalize normal vector
		normalLength = sqrtf(faceNormals->at(i).x*faceNormals->at(i).x + faceNormals->at(i).y*faceNormals->at(i).y +
							faceNormals->at(i).z*faceNormals->at(i).z);

		faceNormals->at(i).x /= normalLength;
		faceNormals->at(i).y /= normalLength;
		faceNormals->at(i).z /= normalLength;
	}
}

// Private function: Calculate and populate the vertexNormals
void terrain::calcVertexNormals() {
	float aveX, aveY, aveZ;
	// Normal for each vertex is the average of the normals of the (1-4) faces around it
	for (int i = 0; i < verts->size(); ++i) {
		aveX = 0;
		aveY = 0;
		aveZ = 0;

		for (int j = 0; j < 4; ++j) {
			if (verts->at(i).faces[j] != -1) {
				aveX += faceNormals->at(verts->at(i).faces[0]).x;
				aveY += faceNormals->at(verts->at(i).faces[0]).y;
				aveZ += faceNormals->at(verts->at(i).faces[0]).z;
			}
		}
		
		aveX /= verts->at(i).numFaces;
		aveY /= verts->at(i).numFaces;
		aveZ /= verts->at(i).numFaces;

		vertexNormals->at(i).set(aveX,aveY,aveZ);
	}
}

void terrain::drawWall(int v1, int v2) {
	glColor3f(0.65,0.65,0.65);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(verts->at(v1).x, verts->at(v1).y, verts->at(v1).z);
		glTexCoord2f(0, 1);
		glVertex3f(verts->at(v2).x, verts->at(v2).y, verts->at(v2).z);
		glTexCoord2f(1, 0);
		glVertex3f(verts->at(v2).x, verts->at(v2).y + height, verts->at(v2).z);
		glTexCoord2f(1, 1);
		glVertex3f(verts->at(v1).x, verts->at(v1).y + height, verts->at(v1).z);
	glEnd();
}

float* terrain::convertHeightMapToFace(int i, int j) {
	int faceIndex = (i*sizeX) + j;
	float centerPoint[] = {0,0,0};

	centerPoint[0] = (faces->at(faceIndex).v3.x + faces->at(faceIndex).v1.x) / 2;
	centerPoint[2] = (faces->at(faceIndex).v3.z + faces->at(faceIndex).v1.z) / 2;

	centerPoint[1] = mazeHeightMap[i][j];

	return centerPoint;
}