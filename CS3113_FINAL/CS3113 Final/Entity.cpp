#include "Entity.h"

Entity::Entity() : scale(1.0f, 1.0f, 1.0f, 1.0f), isBillboard(false), remove(false) {}

Entity::Entity(float x, float y, float z) : Entity(x, y, z, false) {}

Entity::Entity(float x, float y, float z, bool isBillboard) : position(x, y, z, 1.0f), scale(1.0f, 1.0f, 1.0f, 1.0f), isBillboard(isBillboard), remove(false) {}

Entity::Entity(const Entity& rhs) : matrix(rhs.matrix), position(rhs.position), scale(rhs.scale), rotation(rhs.rotation), isBillboard(rhs.isBillboard), remove(rhs.remove) {}

Entity& Entity::operator=(const Entity& rhs) {
	if (&rhs != this) {
		// matrix recreated every render
		position = rhs.position;
		scale = rhs.scale;
		rotation = rhs.rotation;
		isBillboard = rhs.isBillboard;
		remove = rhs.remove;
	}

	return *this;
}

void Entity::buildMatrix() {
	Matrix rotateX, rotateY, rotateZ, scaleMat;

	matrix.identity();

	matrix.ml[12] = position.x;
	matrix.ml[13] = position.y;
	matrix.ml[14] = position.z;

	rotateX.m[1][1] = cosf(rotation.x);
	rotateX.m[1][2] = sinf(rotation.x);
	rotateX.m[2][1] = -sinf(rotation.x);
	rotateX.m[2][2] = cosf(rotation.x);

	rotateY.m[0][0] = cosf(rotation.y);
	rotateY.m[2][0] = -sinf(rotation.y);
	rotateY.m[0][2] = sinf(rotation.y);
	rotateY.m[2][2] = cosf(rotation.y);

	rotateZ.m[0][0] = cosf(rotation.z);
	rotateZ.m[0][1] = -sinf(rotation.z);
	rotateZ.m[1][0] = sinf(rotation.z);
	rotateZ.m[1][1] = cosf(rotation.z);

	scaleMat.m[0][0] = scale.x;
	scaleMat.m[1][1] = scale.y;
	scaleMat.m[2][2] = scale.z;

	matrix = scaleMat * (rotateX * rotateY * rotateZ) * matrix;
}