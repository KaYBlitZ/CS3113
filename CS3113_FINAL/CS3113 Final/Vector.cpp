#include "Vector.h"

Vector::Vector() : x(0.0f), y(0.0f), z(0.0f) {}

Vector::Vector(float x, float y, float z) : x(x), y(y), z(z) {}

float Vector::length() {
	return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

void Vector::normalize() {
	float length = this->length();

	if (length == 0.0f) return;

	x /= length;
	y /= length;
	z /= length;
}