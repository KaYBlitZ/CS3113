#pragma once

#include "Vector.h"

class Matrix {
public:
	Matrix();

	Matrix& operator =(const Matrix& rhs);
	
	// the underlying data for 2d array m and 1d array ml are the same
	// that's what union does
	union {
		float m[4][4];
		float ml[16];
	};

	void identity();
	Matrix inverse();
	Matrix operator * (const Matrix &m2);
	Vector operator * (const Vector &v2);
};