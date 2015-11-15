#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <vector>

using namespace std;

struct vertex3D {
	float x, y, z;

	void set(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void show() {
		printf("%f,%f,%f\n", x, y, z);
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

	terrain(int sizeX, int sizeZ, int height);
	void load();
	void draw();

private:
	vector<vertex3D>*verts;
	vector<faces3D>*faces;
	vector<vertex3D>*faceNormals;
	vector<vertex3D>*vertexNormals;
};

#endif