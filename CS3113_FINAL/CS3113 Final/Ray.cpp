#include "Ray.h"

Ray::Ray() {}

Ray::Ray(const Vector &origin, const Vector &direction) : origin(origin), direction(direction) {}

Ray& Ray::operator=(const Ray &rhs) {
	if (this != &rhs) {
		origin = rhs.origin;
		direction = rhs.direction;
	}
	return *this;
}

void Ray::normalize() {
	direction.normalize();
}