#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "terrain.h"
#include "maze2d.h"

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
}

void terrain::load() {
	maze2d maze2d(sizeX,sizeZ,height);
	maze2d.generate();
	maze2d.show();

	// Use maze2d.mazeHeightMap[][] for height (y-coords) of each vertex
	// ...
	// Populate verts, faces, faceNormals and vertexNormals arrays
	// ...
}

// Draw the terrain using glut functions
void terrain::draw() {

}