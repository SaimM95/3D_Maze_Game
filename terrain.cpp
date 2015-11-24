#include <stdio.h>
#include <stdlib.h>
#include <vector>
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
}

// Draw the terrain using glut functions
void terrain::draw() {

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