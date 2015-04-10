#pragma once

#include <cmath>

class Vector {
public:
	Vector();
	Vector(float x, float y, float z);
	float length();
	void normalize();

	float x, y, z;
};