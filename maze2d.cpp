#include <stdio.h>
#include <stdlib.h>
#include "maze2d.h"

// Constructor
// sizeX, sizeZ and height of the maze (walls)
maze2d::maze2d(int sizeX, int sizeZ, int height) {
	this->sizeX = sizeX;
	this->sizeZ = sizeZ;
	this->height = height;

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

// Populate mazeHeightMap array
void maze2d::generate() {
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

void maze2d::show() {
	for (int x = 0; x < sizeX; ++x) {
		for (int z = 0; z < sizeZ; ++z) {
			printf("%d ", mazeHeightMap[x][z]);
		}
		printf("\n");
	}
}