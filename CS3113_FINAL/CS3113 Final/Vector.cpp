#include "Vector.h"

Vector::Vector() : x(0.0f), y(0.0f), z(0.0f), t(0.0f) {}

Vector::Vector(float x, float y, float z, float t) : x(x), y(y), z(z), t(t) {}

Vector::Vector(const Vector &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), t(rhs.t) {}

Vector& Vector::operator=(const Vector &rhs) {
	if (this != &rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}

	return *this;
}

Vector Vector::operator+(const Vector &rhs) {
	return Vector(x + rhs.x, y + rhs.y, z + rhs.z, t + rhs.t);
}

Vector Vector::operator-(const Vector &rhs) {
	return Vector(x - rhs.x, y - rhs.y, z - rhs.z, t - rhs.t);
}

Vector Vector::operator*(const float &scalar) {
	return Vector(scalar * x, scalar * y, scalar * z, scalar * t);
}

float Vector::dot(const Vector &rhs) {
	return x * rhs.x + y * rhs.y + z * rhs.z + t * rhs.t;
}

Vector Vector::cross(const Vector &rhs) {
	return Vector(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y  * rhs.x, 0.0f);
}

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