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
    /* sizeX = (sizeX%2==0)? (sizeX+1):(sizeX); */
    /* sizeZ = (sizeZ%2==0)? (sizeZ+1):(sizeZ); */

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
void terrain::load(CCamera * cam) {
    int startX, startZ, endX, endZ;
	generateMaze(cam, &startX,&startZ,&endX,&endZ);
	showMaze();
	// Use maze2d.mazeHeightMap[][] for height (y-coords) of each vertex
	// ...
	// Populate verts, faces, faceNormals and vertexNormals arrays
	// ...

	// Set x, y, z values for all the vertices in such a way that it translates into a flat plane of points
	// i.e. all points have height 0
	int vertexCount = 0;
	/* for (int x = (-sizeX/2)*10; x <= (sizeX/2)*10; x+=10) { */
	/* 	for (int z = (sizeZ/2)*10; z >= (-sizeZ/2)*10; z-=10) { */
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

		heightZ++;
		if (heightZ >= sizeZ) {
			heightX++;
			heightZ = 0;
		}
	}

	calcFaceNormals();
	calcVertexNormals();

    printf("startX:%i, startZ:%i\n",startX, startZ);
    float camPosX, camPosZ;
    convertHeightMapToFace2(7, 9, &camPosX, &camPosZ);
    cam->setPosition(camPosX, 2, camPosZ);
    printf("cameraPosition:(%f,%f,%f)\n", cam->Position.x, cam->Position.y, cam->Position.z);


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

void terrain::generateMaze(CCamera *cam, int * startXp, int*startZp, int*endXp, int* endZp){
  // make everything equal to -1
  for (int x = 0; x < sizeX; ++x) {
    for (int z = 0; z < sizeZ; ++z) {
      mazeHeightMap[x][z] = 5;
    }
  }

  //base as in the lowest value that has passed recursion
  int base = 11;
  // make all the boundries along the x aixs
  for(int i = 0; i < sizeX; i++){
    mazeHeightMap[i][0]       = 16;
    mazeHeightMap[i][sizeZ-1] = 16;
  }
  // make all the boundries along the y axis
  for(int i = 0; i < sizeZ; i++){
    mazeHeightMap[0][i]       = 16;
    mazeHeightMap[sizeX-1][i] = 16;
  }

  //start adding walls
  int startX, startZ;
  if(rand()%2 > 0){
    startX = (rand()%(sizeX/2-1)) * 2 + 1;
    startZ = (rand()%2 > 0)? 0:sizeZ-1;
  }else{
    startX = (rand()%2 > 0)? 0:sizeX-1;
    startZ = (rand()%(sizeZ/2-1)) * 2 + 1;
  }
  mazeHeightMap[startX][startZ] = 11;
  /* showMaze(); */



  // starting the DFS
  int xStep = startX, zStep = startZ;
  if(xStep == 0 || xStep == sizeX-1)      xStep += 1 - 2*(xStep)/(sizeX-1);
  else if(zStep == 0 || zStep == sizeZ-1) zStep += 1 - 2*(zStep)/(sizeZ-1);
  mazeHeightMap[xStep][zStep] = 11;
  startDFS(xStep, zStep);

  //make the exit
  int endX = 0, endZ = 0;
  if(rand()%2 > 0){
    endX = (rand()%(sizeX/2-1)) * 2 + 1;//rand()%(sizeX-2)+1;
    endZ = (rand()%2 > 0)? 0:sizeZ-1;
  }else{
    endX = (rand()%2 > 0)? 0:sizeX-1;
    endZ = (rand()%(sizeZ/2-1)) * 2 + 1;//rand()%(sizeZ-2)+1;
  }
  /* printf("endX:%i, endZ:%i\n",endX,endZ); */
  mazeHeightMap[endX][endZ] = 11;
  *startXp = startX;
  *startZp = startZ;
  printf("endX:%i, endZ:%i\n",endX, endZ);

  if(endX == sizeX-1 || endZ == sizeZ-1){
    xStep = endX, zStep = endZ;
    if(xStep == 0 || xStep == sizeX-1)      xStep += 1 - 2*(xStep)/(sizeX-1);
    else if(zStep == 0 || zStep == sizeZ-1) zStep += 1 - 2*(zStep)/(sizeZ-1);
    mazeHeightMap[xStep][zStep] = 11;
  }
  *endXp = endX;
  *endZp = endZ;
  /* showMaze(); */


  // make all the numbers normal, from 0 or 5
  for (int i = 0; i < sizeX; i++) {
    for (int j = 0; j < sizeZ; j++){
      if(mazeHeightMap[i][j]==-1) mazeHeightMap[i][j] = 0;
      else if(mazeHeightMap[i][j] >= 10) mazeHeightMap[i][j] = mazeHeightMap[i][j]-11; // so then -1 = 10, 0 = 11, 12 = 1
    }
  }

  /* printf("done the start making terrain function\n"); */
}
void terrain::startDFS(int x, int z){
  mazeHeightMap[x][z] = 11;
  /* showMaze(); */
  // all the possible values
    // x+1 , z   = right
    // x+1 , z+1
    // x   , z+1 = down
    // x-1 , z+1
    // x-1 , z   = left
    // x-1 , z-1
    // x   , z-1 = top
    // x+1 , z-1
  // end of all the possible values
  /* int possibleX[] = {x+1 , x   , x-1 , x   }; */
  /* int possibleZ[] = {z   , z+1 , z   , z-1 }; */
  int possibleX[] = {x+2 , x   , x-2 , x   };
  int possibleZ[] = {z   , z+2 , z   , z-2 };

  // make a loop that goes through all of em
  int numberOfPlaces = 0;
  for(int i = 0; i < sizeof(possibleX)/sizeof(possibleX[0]); i++){
    int curX = possibleX[i];
    int curZ = possibleZ[i];
    if(curX >= sizeX-1 || curZ >= sizeZ-1 || curX <= 0 || curZ <= 0) continue;
    if(mazeHeightMap[curX][curZ] >= 10) continue;
    numberOfPlaces++;
  }

  if(numberOfPlaces == 0) return;
  int chosenPath = rand()%numberOfPlaces;
  numberOfPlaces = 0;
  for(int i = 0; i< sizeof(possibleX)/sizeof(possibleX[0]); i++){
    int curX = possibleX[i];
    int curZ = possibleZ[i];
    if(curX >= sizeX-1 || curZ >= sizeZ-1 || curX <= 0 || curZ <= 0) continue;
    if(mazeHeightMap[curX][curZ] >= 10) continue;
    if(chosenPath == numberOfPlaces){
      //chose this path
      int dirX = curX-x;//x-curX;
      dirX = (dirX!=0)? abs(dirX)/dirX:0;
      int dirZ = curZ-z;//z-curZ;
      dirZ = (dirZ!=0)? abs(dirZ)/dirZ:0;
      mazeHeightMap[x+dirX     ][z+dirZ     ] = 11;
      startDFS(curX,curZ);
      startDFS(x,z);
      return;
    }
    numberOfPlaces++;
  }

  //end of function
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
    printf("centerPoint:(%f,%f,%f)\n", centerPoint[0], centerPoint[1], centerPoint[2]);

	return centerPoint;
}

void terrain::convertHeightMapToFace2(int i, int j, float *posX, float*posZ) {
	int faceIndex = (i*sizeX) + j;

	*posX = (faces->at(faceIndex).v3.x + faces->at(faceIndex).v1.x) / 2;
	*posZ = (faces->at(faceIndex).v3.z + faces->at(faceIndex).v1.z) / 2;
}

