#include "Billboard.h"

Billboard::Billboard() {}

Billboard::Billboard(float x, float y, float z) : Entity(x, y, z, true) {}

Billboard::Billboard(float x, float y, float z, const Sprite &sprite) : Entity(x, y, z, true), sprite(sprite) {}

Billboard::Billboard(const Billboard &rhs) : Entity(rhs), sprite(rhs.sprite) {}

Billboard& Billboard::operator=(const Billboard &rhs) {
	if (this != &rhs) {
		Entity::operator=(rhs);
		sprite = rhs.sprite;
	}
	return *this;
}

// create a new coordinate system where the billboard is facing the camera
// simply just draw at that point
void Billboard::buildMatrix() {
	Vector y(0.0f, 1.0f, 0.0f, 1.0f); // this is fine since the game is played on the same plane
	// vector z is vector facing the camera, which is stored in rotation vector
	Vector x = y.cross(rotation);
	x.normalize();
	rotation.normalize();

	matrix.identity();
	matrix.ml[0] = x.x;
	matrix.ml[1] = x.y;
	matrix.ml[2] = x.z;
	matrix.ml[4] = y.x;
	matrix.ml[5] = y.y;
	matrix.ml[6] = y.z;
	matrix.ml[8] = rotation.x;
	matrix.ml[9] = rotation.y;
	matrix.ml[10] = rotation.z;
	matrix.ml[12] = position.x;
	matrix.ml[13] = position.y;
	matrix.ml[14] = position.z;

	Matrix scaleMat;
	scaleMat.ml[0] = scale.x;
	scaleMat.ml[5] = scale.y;
	scaleMat.ml[10] = scale.y;

	matrix = scaleMat * matrix;
}

void Billboard::fixedUpdate() {}

void Billboard::update() {
	buildMatrix();
}

void Billboard::render() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.ml);

	sprite.draw();

	glPopMatrix();
}