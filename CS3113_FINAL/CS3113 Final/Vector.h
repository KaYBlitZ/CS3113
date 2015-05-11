#pragma once

#include <cmath>

class Vector {
public:
	Vector();
	Vector(float x, float y, float z, float t);
	Vector(const Vector &rhs);
	Vector& operator=(const Vector &rhs);
	Vector operator+(const Vector &rhs);
	Vector operator-(const Vector &rhs);
	Vector operator*(const float &scalar);

	float dot(const Vector &rhs);
	Vector cross(const Vector &rhs);
	float length();
	void normalize();

	float x, y, z, t;
};