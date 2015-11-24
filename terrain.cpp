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
	// NOTE THE ORDER OF sizeX,sizeZ (i.e. (x,z)) in array
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
	int heightX, heightZ;
	for (int x = -sizeX/2; x <= sizeX/2; ++x) {
		heightX = x + (sizeX/2);
		for (int z = sizeZ/2; z >= -sizeZ/2; --z) {
			heightZ = (sizeZ/2) - z;
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
	}

	calcFaceNormals();
	calcVertexNormals();
}

// Draw the terrain using glut functions
void terrain::draw() {
	int xCount = 0;

	// use quads or triangles to draw the things in your picture
	for (int i = 0; i < faces->size(); ++i) {
		if (i > 0 && i%sizeX == 0) xCount++;
		int v1 = i+sizeZ+1+xCount;
		int v2 = i+sizeZ+2+xCount;
		int v3 = i+1+xCount;
		int v4 = i+0+xCount;

		glColor3f(0,1,1);
	
		glBegin(GL_POLYGON);
			glNormal3f(vertexNormals->at(v1).x, vertexNormals->at(v1).y, vertexNormals->at(v1).z);
			glVertex3f(faces->at(i).v1.x, faces->at(i).v1.y, faces->at(i).v1.z);

			glNormal3f(vertexNormals->at(v2).x, vertexNormals->at(v2).y, vertexNormals->at(v2).z);
			glVertex3f(faces->at(i).v2.x, faces->at(i).v2.y, faces->at(i).v2.z);

			glNormal3f(vertexNormals->at(v3).x, vertexNormals->at(v3).y, vertexNormals->at(v3).z);
			glVertex3f(faces->at(i).v3.x, faces->at(i).v3.y, faces->at(i).v3.z);

			glNormal3f(vertexNormals->at(v4).x, vertexNormals->at(v4).y, vertexNormals->at(v4).z);
			glVertex3f(faces->at(i).v4.x, faces->at(i).v4.y, faces->at(i).v4.z);
		glEnd();
	}
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

	// Generate maze walls
	// ...
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