#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"
#include "GL/glut.h"
#include <math.h>

class Mesh
{
public:
	Vec3<double> _minPos;
	Vec3<double> _maxPos;
	vector<Vertex*> _vertices;
	vector<Face*> _faces;
public:
	Mesh(const char* filename) {
		open(filename);
	}
	~Mesh() {}
public:
	void open(const char* filename);
	void computeNormal();
	void BuildList();
	void moveToCenter(double scailing);
	void reset();
public:
	void smoothing(int iter = 1);
	void laplacianSmoothing(int iter = 1, double lam = 0.5);
	void taubinSmoothing(int iter = 1, double lam = 0.5, double mu = -0.4);
	void cotanSmoothing(int iter = 1);
public:
	void drawPoint();
	void drawWire();
	void drawSurface(bool smooth = false);
};

#endif