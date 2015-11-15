#ifndef _MAZE2D_H_
#define _MAZE2D_H_

class maze2d {
public:
	int sizeX, sizeZ, height;
	int **mazeHeightMap; 
	
	maze2d(int sizeX, int sizeZ, int height);
	void generate();
	void show();
};

#endif