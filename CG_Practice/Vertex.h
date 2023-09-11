#ifndef __VERTEX__H__
#define __VERTEX__H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Vertex
{
public:
	int _index;
	Vec3<double> _pos;
	Vec3<double> _pos0;
	Vec3<double> _normal;
	vector<Vertex*> _nbVertices;
	vector<Face*> _nbFaces;
public:
	Vertex(int index, Vec3<double> p) {
		_index = index;
		_pos = p;
		_pos0 = p;
	}
	~Vertex() {}
public:
	inline void x(double value) { _pos[0] = value; }
	inline void y(double value) { _pos[1] = value; }
	inline void z(double value) { _pos[2] = value; }
	inline double x() { return _pos[0]; }
	inline double y() { return _pos[1]; }
	inline double z() { return _pos[2]; }
};

#endif