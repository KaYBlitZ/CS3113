#include "Bullet.h"

const float Bullet::VELOCITY = 1.0f;
const float Bullet::TIME_ALIVE = 1.5f;

Bullet::Bullet() : Entity(0.0f, 0.0f, 0.0f), timeAlive(0.0f) {}

Bullet::Bullet(float x, float y, float rotation, const Vector& playerVelocity) : Entity(x, y, rotation), timeAlive(0.0f) {
	velocity.x = playerVelocity.x + VELOCITY * cos(rotation * Entity::degToRadRatio);
	velocity.y = playerVelocity.y + VELOCITY * sin(rotation * Entity::degToRadRatio);
}

void Bullet::fixedUpdate() {
	x += velocity.x * FIXED_TIMESTEP;
	y += velocity.y * FIXED_TIMESTEP;

	if (x > 1.33f) {
		x = -1.33f;
	}
	else if (x < -1.33f) {
		x = 1.33f;
	}

	if (y > 1.0f) {
		y = -1.0f;
	}
	else if (y < -1.0f) {
		y = 1.0f;
	}

	timeAlive += FIXED_TIMESTEP;

	if (timeAlive > TIME_ALIVE) {
		remove = true;
	}
}

void Bullet::render() {
	createMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(matrix.ml);

	GLfloat point[2] = { 0.0f, 0.0f };

	glVertexPointer(2, GL_FLOAT, 0, point);
	glEnableClientState(GL_VERTEX_ARRAY);

	glPointSize(5.0f);

	glDrawArrays(GL_POINTS, 0, 1);

	glPopMatrix();
}