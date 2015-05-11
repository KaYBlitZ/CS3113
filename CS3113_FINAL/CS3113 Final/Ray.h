#pragma once

#include "Vector.h"

class Ray {
public:
	Ray();
	Ray(const Vector &origin, const Vector &direction);
	Ray& operator=(const Ray &rhs);

	void normalize();

	Vector origin, direction;
};